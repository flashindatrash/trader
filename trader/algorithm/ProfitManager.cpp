#include "ProfitManager.hpp"
#include "Logger.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/DecisionMaker.hpp"

// мин % соотношение, меньше которого не сливать
static Change sMinRate = 0.0035;

// желаемое % соотношение
static Change sRate = 0.006;

// хвостик в % цены, если меньше, то ждем, не закрываем позицию
static Change sStrongTail = 0.0015;

ProfitManager::ProfitManager(OrderManager& orders)
    : BaseManager(orders)
{
}

void ProfitManager::tick(const Symbol& symbol) {
    const CandlestickWrapper* candlestick = Exchanger().chart()->last();
    if (candlestick == nullptr)
        return;

    // находим ордер для закрытия
    const OrderWrapper* position = findClosableOrder(symbol);
    if (position == nullptr)
        return;

    // создаем реквест
    OrderRequest request;
    request.side = revertSide(position->side());
    request.quantity = position->quantity();

    // если хвостик слабенький, то ждем
    if (request.side == OrderSide::Sell && candlestick->isBullish()) {
        Change tail = candlestick->wickLen() / symbol.getPrice();
        if (tail < sStrongTail) {
            Logger::info("wait to sell... %f", symbol.getPrice());
            return;
        }
    } else if (request.side == OrderSide::Buy && candlestick->isBearish()) {
        Change tail = candlestick->tailLen() / symbol.getPrice();
        if (tail < sStrongTail) {
            Logger::info("wait to buy... %f", symbol.getPrice());
            return;
        }
    }

    // пробуем создать новый ордер
    _orders.create(request, position);
}

const OrderWrapper* ProfitManager::findClosableOrder(const Symbol &symbol) const {
    DecisionMaker decision(symbol);

    // найдем ордер, который стоит закрыть по более выгодному курсу
    const OrderWrapper* transaction = nullptr;
    double best_change = 0.0;
    for (const OrderWrapper* order : _orders.getPositions()) {
        Change change = util::change(order->getPrice(), symbol.getPrice());

        // открытая позиция соответствует сайду
        const OrderSide revert = change > 0.0 ? OrderSide::Sell : change < 0.0 ? OrderSide::Buy : OrderSide::Invalid;
        if (revert == OrderSide::Invalid || revert == order->side())
            continue;

        // не продаем меньше, чтобы профит перекрывал комисию
        if (std::abs(change) < sMinRate)
            continue;

        double factor = decision.factor(revert, DecisionMaker::ForProfit);
        if (std::abs(change) / sRate * factor < 1.0)
            continue;

        if (std::abs(change) < best_change)
            continue;

        // проверим, что достаточн средств для закрытия ордера
        OrderRequest request;
        request.side = revert;
        request.quantity = order->quantity();
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
