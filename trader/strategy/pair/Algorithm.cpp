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
    if (change < profitable->price() * _settings.close_position_percent)
        return false;

    // проверим достаточно ли средств для сделки
    request.side = OrderWrapper::revert(profitable->side());
    request.quantity = profitable->quantity();
    if (not request.isEnough())
        return false;

    if (_settings.test) {
        static double sumprofit = 0.0;
        sumprofit += change * profitable->quantity();
        Logger::info("close %d position, profit summary %f USDT", profitable->side(), sumprofit);
        _positions->remove(*profitable);
        return true;
    }

    return _positions->create(request);
}

bool Algorithm::tryOpenPosition(const Context& context) {
    OrderRequest request;
    request.symbol = _settings.symbol;

    // выбираем куда идем в шорт или лонг
    // пока довольно примитивно по направлению свечи
    Change change = util::change(context.candlestick->priceOpen(), context.candlestick->priceClose());
    request.side = change > 0.0 ? OrderSide::Sell : change < 0.0 ? OrderSide::Buy : OrderSide::Invalid;
    if (request.side == OrderSide::Invalid)
        return false;

    if (request.side == OrderSide::Buy)
        int i =  0;

    // найдем последнюю лонг или шорт позицию
    // проверим, можно ли нам войти еще раз в нее же
    // и определим сумму лота новой позиции
    const Positions::const_iterator last = _positions->last(request.side);
    if (last == _positions->cend()) {
        // это первая позиция в шорте или лонге, откроем ее с минимальным лотом
        request.quantity = Exchanger().minQuantity(request.symbol);
    } else if (last->distance(context.price()) < -last->price() * _settings.open_next_price_percent) {
        // ближаяшая должна иметь дистанцию больше допустимого шага
        // открываем лот с умножением на коэфициент из конфига
        request.quantity = last->quantity() * _settings.open_next_lot_multiply;
    } else {
        // ближашая позиция уже имеет профит, или дистанция меньше допустимого шага
        // дождемся получения прибыли с нее, или изменению в проигрышную сторону
        return false;
    }

    // проверим, что количество открытых сделок не превышает установленный лимит средств
    // нас итересует только конкретная одна валюта, с которой собираемся оперировать
    const Quantity max_quantity = request.side == OrderSide::Sell ? _settings.balance_base_limit : _settings.balance_quote_limit;
    if (max_quantity >= 0.0) {
        const Quantity sum_quantity = _positions->summarize<Quantity>(Predicates::side(request.side), Summarizes::quantity);
        // добавим к общей суммарному вкладу открытых позиций и ту, которую хотим добавить
        // bug: buy FUN 397 q > 200$
        if (sum_quantity + request.quantity > max_quantity)
            return false;
    }

    // проверим, что средств достаточно для закрытия ближайшего противопложного шорта + лонга
    const Positions::const_iterator last_opposite = _positions->last(OrderWrapper::revert(request.side));
    if (last_opposite != _positions->cend()) {
        // возьмем текущий баланс в base ассете - отнимем количество, которое хотим выставить
        // и оно должно быть выше > или = противоположной ставки
        Quantity balance_in_base = last_opposite->side() == OrderSide::Buy ? request.symbol.baseAsset().getBalance() : request.symbol.quoteAsset().getBalance() / context.price();
        if (balance_in_base - request.quantity < last_opposite->quantity())
            return false;
    }

    // убедимься, что достаточно средств для сделки
    if (not request.isEnough())
        return false;

    if (_settings.test) {
        Logger::info("add %d position for %f change %f", request.side, context.price(), change);
        Position test("test" + std::to_string(_positions->size()));
        test.setPrice(context.price());
        test.setQuantity(request.quantity);
        test.setSide(request.side);
        return _positions->push(test);
    }

    return _positions->create(request);
}
