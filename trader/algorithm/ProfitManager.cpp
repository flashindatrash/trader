#include "ProfitManager.hpp"
#include "Logger.hpp"
#include "proxy/BinanceOrders.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/binance/response/BinanceKlineData.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/DecisionMaker.hpp"

// мин % соотношение, меньше которого не сливать
static float sMinRate = 0.0035f;

// желаемое % соотношение
static float sRate = 0.005f;

ProfitManager::ProfitManager(OrderManager& orders)
    : BaseManager(orders)
{
}

void ProfitManager::tick(const Symbol& symbol) {
    // находим ордер для закрытия
    const BinanceOrderData* transaction = findClosableOrder(symbol);
    if (transaction == nullptr)
        return;

    // пробуем создать новый ордер
    _orders.create(symbol, transaction->side.reverse(), transaction->quantity, transaction);
}

const BinanceOrderData* ProfitManager::findClosableOrder(const Symbol &symbol) const {
    DecisionMaker decision(symbol);

    // найдем ордер, который стоит закрыть по более выгодному курсу
    const BinanceOrderData* transaction = nullptr;
    double best_change = 0.0;
    for (const BinanceOrderData& order : _orders.getPositions()) {
        Change change = PriceRange(order.getPrice(), symbol.getPrice()).change();

        // открытая позиция соответствует сайду
        if (SideEnum(change) == order.side)
            continue;

        // не продаем меньше, чтобы профит перекрывал комисию
        if (std::abs(change) < sMinRate)
            continue;

        double factor = decision.factor(change, DecisionMaker::ForProfit);
        if (std::abs(change) / sRate * factor < 1.0)
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
