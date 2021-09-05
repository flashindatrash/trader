#include "Algorithm.hpp"

#include <utility>
#include <Time.hpp>
#include "Logger.hpp"
#include "Context.hpp"
#include "Positions.hpp"
#include "Statistics.hpp"
#include "Status.hpp"
#include "Migrator.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/Exchanger.hpp"

NS_USE

Algorithm* Algorithm::create(const Settings& settings) {
    auto* algorithm = new Algorithm(settings);
    return algorithm;
}

Algorithm::Algorithm(Settings settings)
    : _settings(std::move(settings))
{
}

Algorithm::~Algorithm() {
    if (_positions != nullptr) {
        delete _positions;
        _positions = nullptr;
    }

    if (_statistics != nullptr) {
        delete _statistics;
        _statistics = nullptr;
    }
}

bool Algorithm::init() {
    Status::setTitle(_settings.symbol);
    Logger::setLogfile("/tmp/" + _settings.uniqId() + ".log");

    // создадим структуры для хранения данных
    _positions = Positions::create(_settings.uniqId() + ":positions", not _settings.test);
    _statistics = Statistics::create(_settings.uniqId() + ":stats", not _settings.test);

    // промигрируем данные
    Migrator::migrate(_positions, _statistics, _settings.symbol, _settings.test);
    return true;
}

void Algorithm::execute(const Context& context) {
    bool close = tryClosePosition(context);
    bool open = tryOpenPosition(context);

    Status::update(*_positions, _settings, context);
}

bool Algorithm::tryClosePosition(const Context& context) {
    OrderRequest request;
    request.symbol = _settings.symbol;

    // самая выгодная лонг или шорт позиция
    const auto profitable = _positions->compare_if(Predicates::closable(request.symbol),
                                                   Compares::profitable(context.price()));
    if (profitable == _positions->cend())
        return false;

    // интересует только те, у которых изменилась цена выше указанного порога
    if (profitable->distance(context.price()) < profitable->price() * _settings.close_position_percent)
        return false;

    // проверим достаточно ли средств для сделки
    request.side = OrderUtil::revert(profitable->side());
    request.quantity = profitable->baseQuantity();

    // настройка ожидания слабого хвостика
    if (_settings.strong_tail_percent > 0.0) {
        // новый хвостик, ждем N времен
        if (Time().ms() < context.candlestick->timeOpen() + Timer::sSecond * 5)
            return false;

        // если хвостик слабенький, то ждем
        if ((request.side == OrderSide::Sell &&
             context.candlestick->isBullish() &&
             context.candlestick->wickLen() / context.price() < _settings.strong_tail_percent) ||
            (request.side == OrderSide::Buy &&
             context.candlestick->isBearish() &&
             context.candlestick->tailLen() / context.price() < _settings.strong_tail_percent)) {
            return false;
        }
    }

    // создание заказа
    Quantity profit;
    if (_settings.test) {
        profit = profitable->profit(context.price());
    } else {
        const OrderWrapper* result = Exchanger().createOrder(request);
        if (result == nullptr)
            return false;

        Status::printOrder(*result, profitable->id(), "<");

        // профит = дистанция между 2мя ценами * лот - коммисия 1й и 2й сделки
        profit = profitable->profit(result->price()) - result->fee();
    }

    // удалим из базы, результат удаления не важен
    _positions->remove(*profitable);

    // пытаемся закрыть другие позиции
    while(true) {
        // найдем позицию с максимальным отставанием, которую можем закрыть
        // её убыток (отрицательный профит) должен быть больше полученного данной сделкой
        const auto losable = _positions->compare_if(Predicates::profitGreater(context.price(), -profit),
                                                       Compares::losable(context.price()));
        if (losable == _positions->cend())
            break;

        Status::printOrder(*losable, losable->id(), "*");

        // вычитаем профит (будет отрицательный) и удаляем позицию
        profit += losable->profit(context.price());
        _positions->remove(*losable);
    }

    // сохраняем профит, высчитываем и показываем PNL
    auto profits = _statistics->addProfit(profit);
    auto losses = _positions->summarize<Quantity>(Summarizes::profit(context.price()));

    Status::printProfit(profit, profits, losses);
    return true;
}

bool Algorithm::tryOpenPosition(const Context& context) {
    OrderRequest request;
    request.symbol = _settings.symbol;

    // выбираем куда идем в шорт или лонг
    // пока довольно примитивно по направлению свечи
    Change change = OrderUtil::change(context.candlestick->priceOpen(), context.candlestick->priceClose());
    request.side = change > 0.0 ? OrderSide::Sell : change < 0.0 ? OrderSide::Buy : OrderSide::Invalid;
    if (request.side == OrderSide::Invalid)
        return false;

    // найдем последнюю лонг или шорт позицию
    // проверим, можно ли нам войти еще раз в нее же
    // и определим сумму лота новой позиции
    const auto last = _positions->last(request.side);
    if (last == _positions->cend()) {
        // это первая позиция в шорте или лонге, откроем ее с минимальным лотом
        request.quantity = Exchanger().minQuantity(request.symbol);
    } else if (last->distance(context.price()) < -last->price() * _settings.price_distance) {
        // ближаяшая должна иметь дистанцию больше допустимого шага
        // открываем лот с умножением на коэфициент из конфига
        request.quantity = last->baseQuantity() * _settings.open_lot_multiply;
        // но с ограничением в максимум
        if (_settings.open_max_multiply > 1.0)
            request.quantity = std::min(request.quantity, Exchanger().minQuantity(request.symbol) * _settings.open_max_multiply);
    } else if (not OrderUtil::isEnough(request.symbol, OrderUtil::revert(request.side), last->baseQuantity())) {
        // произошла беда, мы потратили все деньги, и не можем закрыть сделку
        request.quantity = last->baseQuantity();
    } else {
        // ближашая позиция уже имеет профит, или дистанция меньше допустимого шага
        // дождемся получения прибыли с нее, или изменению в проигрышную сторону
        Logger::trace("open: waiting profitable [%d]", last->side());
        return false;
    }

    // проверим, что средств достаточно для закрытия ближайшего противопложного шорта + лонга
    // еще обязательным условием, что он находится в достигаемом диапазоне, иначе бот блокируется при 0 балансе
    const auto last_opposite = _positions->last(OrderUtil::revert(request.side));
    if (last_opposite != _positions->cend() && OrderUtil::changeAbs(context.price(), last_opposite->price()) < _settings.close_position_percent) {
        // баланс, который используется для закрытия противоположной сделки
        Quantity balance = OrderUtil::spentQuantity(last_opposite->side(), request.symbol.baseAsset().balance(),
                                                    request.symbol.quoteAsset().balance());
        // посчитаем расход данной сделки
        Quantity request_quantity = OrderUtil::spentQuantity(request.side, request.quantity,
                                                             request.quantity * context.price());

        // баланс должен быть выше суммы сделки на закрытие и оперируемой
        Quantity frozen = last_opposite->spentQuantity() + request_quantity;
        if (balance < frozen) {
            Logger::trace("open: balance (%f) < frozen (%f)", balance, frozen);
            return false;
        }
    }

    if (_settings.test) {
        static int sTestId = 1;
        Position test("test" + std::to_string(++sTestId));
        test.setBaseQuantity(request.quantity);
        test.setQuoteQuantity(request.quantity * context.price());
        test.setSide(request.side);
        if (not _positions->push(test)) {
            Logger::trace("open: failed to push");
            return false;
        }
    } else {
        const OrderWrapper* result = Exchanger().createOrder(request);
        if (not _positions->copy(result)) {
            Logger::trace("open: failed create %d position with %f quantity", request.side, request.quantity);
            return false;
        }

        Status::printOrder(*result, result->id(), ">");
    }

    return true;
}
