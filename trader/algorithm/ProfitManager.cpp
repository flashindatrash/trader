#include "ProfitManager.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/DecisionMaker.hpp"

// мин % соотношение, меньше которого не сливать
static float sMinRate = 0.0035f;

// желаемое % соотношение
static float sRate = 0.008f;

ProfitManager::ProfitManager(OrderManager& orders)
    : BaseManager(orders)
{
}

void ProfitManager::tick(const Symbol& symbol) {
    // находим ордер для закрытия
    const OrderWrapper* transaction = findClosableOrder(symbol);
    if (transaction == nullptr)
        return;

    // пробуем создать новый ордер
    OrderRequest request;
    request.side = revertSide(transaction->side());
    request.quantity = transaction->quantity();
    _orders.create(request, transaction);
}

const OrderWrapper* ProfitManager::findClosableOrder(const Symbol &symbol) const {
    DecisionMaker decision(symbol);

    // найдем ордер, который стоит закрыть по более выгодному курсу
    const OrderWrapper* transaction = nullptr;
    double best_change = 0.0;
    for (const OrderWrapper* order : _orders.getPositions()) {
        Change change = PriceRange(order->getPrice(), symbol.getPrice()).change();

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
