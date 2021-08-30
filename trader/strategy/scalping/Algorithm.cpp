#include "Algorithm.hpp"

#include <utility>
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
    Logger::setLogfile(_settings.uniqId().c_str());
    _positions = Positions::create(_settings.uniqId() + ":positions", not _settings.test);
    _statistics = Statistics::create(_settings.uniqId() + ":stats", not _settings.test);
    Migrator::migrate(_positions, _statistics, _settings.symbol, _settings.test);
    for (auto it = _positions->cbegin(); it < _positions->cend(); ++it)
        Status::printOrder(*it, "old");

    return true;
}

void Algorithm::execute(const Context& context) {
    bool close = tryClosePosition(context);
    bool open = tryOpenPosition(context);
}

bool Algorithm::tryClosePosition(const Context& context) {
    OrderRequest request;
    request.symbol = _settings.symbol;

    // самая выгодная лонг или шорт позиция
    const auto profitable = _positions->compare(Compares::distance(context.price()));
    if (profitable == _positions->cend()) {
        Logger::trace("close: hasn't profit for price %f", context.price());
        return false;
    }

    // интересует только те, у которых изменилась цена выше указанного порога
    Change distance_current = profitable->distance(context.price());
    Change distance_waited = profitable->price() * _settings.close_position_percent;
    if (distance_current < distance_waited) {
        Logger::trace("close: distance current (%f) < waited(%f)", distance_current, distance_waited);
        return false;
    }

    // проверим достаточно ли средств для сделки
    request.side = OrderWrapper::revert(profitable->side());
    request.quantity = profitable->baseQuantity();

    // создаем заказ (только не в тесте) и запоминаем цену закрытия
    Price closed_price;
    if (_settings.test) {
        closed_price = context.price();
    } else {
        const OrderWrapper* result = Exchanger().createOrder(request);
        if (result == nullptr) {
            Logger::trace("close: failed create %d position with %f quantity", request.side, request.quantity);
            return false;
        }

        Status::printOrder(*result, "close");
        closed_price = result->price();
    }

    // добавим в статистику прибыль, которую получили из закрытой позиции
    Quantity profit = _statistics->addProfit(profitable->distance(closed_price) * request.quantity);
    Status::addProfit(profit);

    // удалим из базы, результат удаления не важен
    _positions->remove(*profitable);

    return true;
}

bool Algorithm::tryOpenPosition(const Context& context) {
    OrderRequest request;
    request.symbol = _settings.symbol;

    // выбираем куда идем в шорт или лонг
    // пока довольно примитивно по направлению свечи
    Change change = OrderUtil::change(context.candlestick->priceOpen(), context.candlestick->priceClose());
    request.side = change > 0.0 ? OrderSide::Sell : change < 0.0 ? OrderSide::Buy : OrderSide::Invalid;
    if (request.side == OrderSide::Invalid) {
        Logger::trace("open: neutral candlestick");
        return false;
    }

    // найдем последнюю лонг или шорт позицию
    // проверим, можно ли нам войти еще раз в нее же
    // и определим сумму лота новой позиции
    const auto last = _positions->last(request.side);
    if (last == _positions->cend()) {
        // это первая позиция в шорте или лонге, откроем ее с минимальным лотом
        request.quantity = Exchanger().minQuantity(request.symbol);
    } else if (last->distance(context.price()) < -last->price() * _settings.open_price_percent) {
        // ближаяшая должна иметь дистанцию больше допустимого шага
        // открываем лот с умножением на коэфициент из конфига
        request.quantity = last->baseQuantity() * _settings.open_lot_multiply;
        // но с ограничением в максимум
        if (_settings.open_max_multiply > 1.0)
            request.quantity = std::min(request.quantity, Exchanger().minQuantity(request.symbol) * _settings.open_max_multiply);
    } else {
        // ближашая позиция уже имеет профит, или дистанция меньше допустимого шага
        // дождемся получения прибыли с нее, или изменению в проигрышную сторону
        Logger::trace("open: waiting profit");
        return false;
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
        if (limit < total + request_expanses) {
            Logger::trace("open: reach limit %d orders", request.side);
            return false;
        }
    }

    // проверим, что средств достаточно для закрытия ближайшего противопложного шорта + лонга
    // еще обязательным условием, что он находится в достигаемом диапазоне, иначе бот блокируется при 0 балансе
    const auto last_opposite = _positions->last(OrderWrapper::revert(request.side));
    if (last_opposite != _positions->cend() && OrderUtil::changeAbs(context.price(), last_opposite->price()) < _settings.close_position_percent) {
        // баланс, который используется для закрытия противоположной сделки
        Quantity balance = OrderUtil::usingQuantity(last_opposite->side(), request.symbol.baseAsset().getBalance(),
                                                    request.symbol.quoteAsset().getBalance());
        // баланс должен быть выше суммы сделки на закрытие и оперируемой
        Quantity frozen = last_opposite->expanses() + request_expanses;
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

        Status::printOrder(*result, "open");
    }

    return true;
}
