#include "Algorithm.hpp"

#include <utility>
#include "Context.hpp"
#include "Positions.hpp"
#include "Statistics.hpp"
#include "Status.hpp"
#include "Migrator.hpp"
#include "Logger.hpp"
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
}

bool Algorithm::init() {
    _positions = Positions::create(_settings.username + ":" + _settings.symbol.id() + ":positions", not _settings.test);
    _statistics = Statistics::create(_settings.username + ":" + _settings.symbol.id() + ":stats", not _settings.test);
    _status = Status::create(_settings.symbol);
    Migrator::migrate(_positions, _statistics, _settings.symbol, _settings.test);
    return true;
}

void Algorithm::execute(const Context& context) {
    _status->update(context, tryClosePosition(context), tryOpenPosition(context));
}

Algorithm::Result Algorithm::tryClosePosition(const Context& context) {
    OrderRequest request;
    request.symbol = _settings.symbol;

    // самая выгодная лонг или шорт позиция
    const auto profitable = _positions->compare(Compares::distance(context.price()));
    if (profitable == _positions->cend())
        return CLOSE_NOT_EXISTS;

    // интересует только те, у которых изменилась цена выше указанного порога
    if (profitable->distance(context.price()) < profitable->price() * _settings.close_position_percent)
        return CLOSE_NON_PROFITABLE;

    // проверим достаточно ли средств для сделки
    request.side = OrderWrapper::revert(profitable->side());
    request.quantity = profitable->baseQuantity();
    if (not request.isEnough())
        return NOT_ENOUGH;

    // создаем заказ (только не в тесте) и запоминаем цену закрытия
    Price closed_price;
    if (_settings.test) {
        closed_price = context.price();
    } else {
        const OrderWrapper* result = Exchanger().createOrder(request);
        if (result == nullptr)
            return FAILED;

        Status::addOrder(*result, "close");
        closed_price = result->price();
    }

    // добавим в статистику прибыль, которую получили из закрытой позиции
    Quantity profit = _statistics->addProfit(profitable->distance(closed_price) * request.quantity);
    Status::addProfit(profit);

    // удалим из базы, результат удаления не важен, просто кинем сообщение
    if (not _positions->remove(*profitable))
        Logger::info("can't delete order %s", profitable->id().c_str());

    return OK;
}

Algorithm::Result Algorithm::tryOpenPosition(const Context& context) {
    OrderRequest request;
    request.symbol = _settings.symbol;

    // выбираем куда идем в шорт или лонг
    // пока довольно примитивно по направлению свечи
    Change change = OrderUtil::change(context.candlestick->priceOpen(), context.candlestick->priceClose());
    request.side = change > 0.0 ? OrderSide::Sell : change < 0.0 ? OrderSide::Buy : OrderSide::Invalid;
    if (request.side == OrderSide::Invalid)
        return INVALID;

    // найдем последнюю лонг или шорт позицию
    // проверим, можно ли нам войти еще раз в нее же
    // и определим сумму лота новой позиции
    const auto last = _positions->last(request.side);
    if (last == _positions->cend()) {
        // это первая позиция в шорте или лонге, откроем ее с минимальным лотом
        request.quantity = Exchanger().minQuantity(request.symbol);
    } else if (last->distance(context.price()) < -last->price() * _settings.open_next_price_percent) {
        // ближаяшая должна иметь дистанцию больше допустимого шага
        // открываем лот с умножением на коэфициент из конфига
        request.quantity = last->baseQuantity() * _settings.open_next_lot_multiply;
    } else {
        // ближашая позиция уже имеет профит, или дистанция меньше допустимого шага
        // дождемся получения прибыли с нее, или изменению в проигрышную сторону
        return OPEN_WAIT_PROFIT;
    }

    // посчитаем расход данной сделки
    Quantity request_expanses = OrderUtil::usingQuantity(request.side, request.quantity, request.quantity * context.price());

    // проверим, что количество открытых сделок не превышает установленный лимит средств
    // нас итересует только конкретная одна валюта, с которой собираемся оперировать
    const Quantity limit = request.side == OrderSide::Sell ? _settings.balance_base_limit : _settings.balance_quote_limit;
    if (limit >= 0.0) {
        const auto total = _positions->summarize<Quantity>(Predicates::side(request.side),
                                                           Summarizes::expanses);
        // добавим к общей суммарному вкладу открытых позиций и ту, которую хотим добавить
        if (limit < total + request_expanses)
            return OPEN_LIMIT;
    }

    // проверим, что средств достаточно для закрытия ближайшего противопложного шорта + лонга
    const auto last_opposite = _positions->last(OrderWrapper::revert(request.side));
    if (last_opposite != _positions->cend()) {
        // баланс, который используется для закрытия противоположной сделки
        Quantity balance = OrderUtil::usingQuantity(last_opposite->side(), request.symbol.baseAsset().getBalance(),
                                                    request.symbol.quoteAsset().getBalance());
        // баланс должен быть выше суммы сделки на закрытие и оперируемой
        if (balance < last_opposite->expanses() + request_expanses)
            return OPEN_PROFIT_SUPPLY;
    }

    // убедимся, что достаточно средств для сделки
    if (not request.isEnough())
        return NOT_ENOUGH;

    if (_settings.test) {
        static int sTestId = 1;
        Position test("test" + std::to_string(++sTestId));
        test.setBaseQuantity(request.quantity);
        test.setQuoteQuantity(request.quantity * context.price());
        test.setSide(request.side);
        if (not _positions->push(test))
            return FAILED;
    } else {
        const OrderWrapper* result = Exchanger().createOrder(request);
        if (not _positions->copy(result)) {
            Logger::info("can't add order");
            return FAILED;
        }

        Status::addOrder(*result, "open");
    }

    return OK;
}
