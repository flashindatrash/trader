#include "ProfitManager.hpp"
#include "OrderManager.hpp"
#include "DecisionMaker.hpp"
#include "Logger.hpp"
#include "proxy/Time.hpp"
#include "proxy/Exchanger.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"

// мин % соотношение, меньше которого не сливать
static Change sMinRate = 0.0035;

// желаемое % соотношение
static Change sRate = 0.006;

// макс хвостик в % от цены, если меньше, то ждем, не закрываем позицию
static Change sMaxTailRate = 0.0015;
// минимально время свечи, после которого применяем правило выше
static time_t sMinTimeCandle = TraderTime::sSecond * 5;

ProfitManager::ProfitManager(OrderManager& orders)
    : BaseManager(orders)
{
}

void ProfitManager::tick(const Symbol& symbol) {
    const CandlestickWrapper* candlestick = Exchanger().chart(symbol)->last();
    if (candlestick == nullptr)
        return;

    // находим ордер для закрытия
    const OrderWrapper* position = findClosableOrder(symbol);
    if (position == nullptr)
        return;

    // создаем реквест
    OrderRequest request;
    request.symbol = symbol;
    request.side = revertSide(position->side());
    request.quantity = position->quantity();

    // новый хвостик, ждем N времен
    if (Time().ms() < candlestick->timeOpen() + sMinTimeCandle)
        return;

    // если хвостик слабенький, то ждем
    if ((request.side == OrderSide::Sell &&
         candlestick->isBullish() &&
         candlestick->wickLen() / symbol.getPrice() < sMaxTailRate) ||
        (request.side == OrderSide::Buy &&
         candlestick->isBearish() &&
         candlestick->tailLen() / symbol.getPrice() < sMaxTailRate))
    {
        if (_temp == 0.0)
            _temp = symbol.getPrice();
        return;
    }

    if (_temp != 0.0) {
        Logger::info("waited candle started at %f, ended at %f", _temp, symbol.getPrice());
        _temp = 0.0;
    }

    // пробуем создать новый ордер
    _orders.create(request, position);
}

const OrderWrapper* ProfitManager::findClosableOrder(const Symbol &symbol) const {
    DecisionMaker decision(_orders.getPositions());

    // найдем ордер, который стоит закрыть по более выгодному курсу
    const OrderWrapper* transaction = nullptr;
    double best_change = 0.0;
    for (const OrderWrapper* order : _orders.getPositions()) {
        Change change = util::change(order->price(), symbol.getPrice());

        // открытая позиция соответствует сайду
        const OrderSide revert = change > 0.0 ? OrderSide::Sell : change < 0.0 ? OrderSide::Buy : OrderSide::Invalid;
        if (revert == OrderSide::Invalid || revert == order->side())
            continue;

        // не продаем меньше, чтобы профит перекрывал комисию
        if (std::abs(change) < sMinRate)
            continue;

        OrderRequest request;
        request.symbol = symbol;
        request.side = revert;
        request.quantity = order->quantity();

        double factor = decision.factor(request, DecisionMaker::ForProfit);
        if (std::abs(change) / sRate * factor < 1.0)
            continue;

        if (std::abs(change) < best_change)
            continue;

        // проверим, что достаточн средств для закрытия ордера
        if (not request.isEnough())
            continue;

        transaction = order;
        best_change = std::abs(change);
    }

    return transaction;
}

OrderSide ProfitManager::revertSide(const OrderSide& original) const {
    if (original == OrderSide::Buy) return OrderSide::Sell;
    if (original == OrderSide::Sell) return OrderSide::Buy;
    return OrderSide::Invalid;
}
