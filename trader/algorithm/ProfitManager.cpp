#include "Logger.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/Database.hpp"
#include "proxy/BinanceOrders.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "algorithm/ProfitManager.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/DecisionMaker.hpp"

// мин % соотношение, может требовать х2 взависимости от факторов
static float sRate = 0.005f;

ProfitManager::ProfitManager(OrderManager& orders)
    : BaseManager(orders, BinanceTime::sSecond * 30)
{
}

bool ProfitManager::check(const TradeSymbol& symbol) {
    if (not BaseManager::check(symbol))
        return false;

    // находим ордер для закрытия
    const BinanceOrderData* transaction = findClosableOrder(symbol);
    if (transaction == nullptr)
        return false;

    // пробуем создать новый ордер
    return _orders.create(symbol, transaction->side.reverse(), transaction->quantity, transaction);
}

const BinanceOrderData* ProfitManager::findClosableOrder(const TradeSymbol &symbol) const {
    DecisionMaker decision(symbol);

    // найдем ордер, который стоит закрыть по более выгодному курсу
    const BinanceOrderData* transaction = nullptr;
    double best_change = 0.0;
    for (const BinanceOrderData& order : _orders.getPositions()) {
        Change change = PriceRange(order.getPrice(), symbol.getPrice()).change();

        // открытая позиция соответствует сайду
        if (BinanceSideEnum(change) == order.side)
            continue;

        if (not decision.make(change / sRate, DecisionMaker::ForProfit))
            continue;

        if (std::abs(change) < best_change)
            continue;

        // проверим, что достаточн средств для закрытия ордера
        if (not SOrders().isEnough(symbol, order.side.reverse(), order.quantity))
            continue;

        transaction = &order;
        best_change = std::abs(change);
    }

    return transaction;
}
