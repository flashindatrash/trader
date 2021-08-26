#include "Algorithm.hpp"
#include "Settings.hpp"
#include "Context.hpp"
#include "Positions.hpp"
#include "Logger.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/Exchanger.hpp"

NS_USE

Algorithm* Algorithm::create(const Settings& settings) {
    Algorithm* algorithm = new Algorithm(settings);
    return algorithm;
}

Algorithm::Algorithm(const Settings& settings)
    : _settings(settings)
{
}

Algorithm::~Algorithm() {
    double losses = 0.0;
    for (Positions::const_iterator it = _positions->cbegin(); it < _positions->cend(); ++it) {
        losses += it->distance(_settings.symbol.getPrice()) * it->quantity();
    }
    Logger::info("loses %f", losses);

    if (_positions != nullptr) {
        delete _positions;
        _positions = nullptr;
    }
}

bool Algorithm::init() {
    _positions = Positions::create(_settings.symbol, not _settings.test);
    return true;
}

void Algorithm::execute(const Context& context) {
    tryClosePosition(context) || tryOpenPosition(context);
}

bool Algorithm::tryClosePosition(const Context& context) {
    OrderRequest request;
    request.symbol = _settings.symbol;

    // самая выгодная лонг или шорт позиция
    const Positions::const_iterator profitable = _positions->compare(Compares::distance(context.price()));
    if (profitable == _positions->cend())
        return false;

    // интересует только те, у которых изменилась цена выше указанного порога
    Change change = profitable->distance(context.price());
    if (change < profitable->price() * 0.0035)
        return false;

    if (_settings.test) {
        static double sumprofit = 0.0;
        sumprofit += change * profitable->quantity();
        Logger::info("close %d position, profit summary %f USDT", profitable->side(), sumprofit);
        _positions->remove(*profitable);
        return true;
    }

    // todo: side
    request.quantity = profitable->quantity();
    return _positions->create(request);
}

bool Algorithm::tryOpenPosition(const Context& context) {
    OrderRequest request;
    request.symbol = _settings.symbol;

    // довольно примитивно пока выбираем по направлению свечи
    Change change = util::change(context.candlestick->priceOpen(), context.candlestick->priceClose());
    request.side = change > 0.0 ? OrderSide::Sell : change < 0.0 ? OrderSide::Buy : OrderSide::Invalid;
    if (request.side == OrderSide::Invalid)
        return false;

    // ближайшая лонг или шорт позиция
    Positions::Compare compare = request.side == OrderSide::Sell ? Compares::max : Compares::min;
    const Positions::const_iterator nearest = _positions->compare_if(Predicates::side(request.side), compare);

    if (nearest == _positions->cend()) {
        // это первая позиция в шорте или лонге, откроем ее с минимальным лотом
        request.quantity = Exchanger().minQuantity(request.symbol);
    } else if (nearest->distance(context.price()) < -nearest->price() * _settings.open_next_price_percent) {
        // ближаяшая должна иметь дистанцию больше допустимого шага
        // открываем лот с умножением на коэфициент из конфига
        request.quantity = nearest->quantity() * _settings.open_next_lot_multiply;
    } else {
        // ближашая позиция уже имеет профит, или дистанция меньше допустимого шага
        // дождемся получения прибыли с нее, или изменению в проигрышную сторону
        return false;
    }

    if (_settings.test) {
        //Logger::info("add %d position for %f change %f", request.side, context.price(), change);

        Position test("test" + std::to_string(_positions->size()));
        test.setPrice(context.price());
        test.setQuantity(request.quantity);
        test.setSide(request.side);
        return _positions->push(test);
    }

    return _positions->create(request);
}
