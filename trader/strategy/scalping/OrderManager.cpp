#include "OrderManager.hpp"
#include "DataManager.hpp"
#include "Logger.hpp"
#include "Time.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/BookWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "util/NumberUtil.hpp"

using namespace scalping;

OrderManager::OrderManager(const Symbol& symbol)
{
    // найдем все открытые позиции
    const std::vector<const OrderWrapper*>& orders = Exchanger().book(symbol)->get();
    std::vector<std::string> keys = DataManager::getPositionIds();
    for (const OrderWrapper* order : orders) {
        std::string id = DataManager::sDbKeyOrder + order->id();
        if (std::find(keys.begin(), keys.end(), id) == keys.end())
            continue;

        _positions.push_back(order);
    }
    sortPositions();

    for (const OrderWrapper* order : _positions)
        printOrder(symbol, order);
    Logger::info("======");
}

bool OrderManager::create(const OrderRequest& request, const OrderWrapper* transaction) {
    // проверяем, что достаточно средств
    if (not request.isEnough())
        return false;

    const OrderWrapper* result = Exchanger().createOrder(request);
    if (result == nullptr)
        return false;

    printOrder(request.symbol, result, transaction);

    // открыть/закрыть транзакцию
    if (transaction == nullptr) {
        DataManager::openPosition(result->id());
        _positions.push_back(result);
    } else {
        std::string order_id = transaction->id();
        DataManager::closePosition(order_id);
        _positions.erase(std::remove_if(_positions.begin(), _positions.end(), [order_id](const OrderWrapper* t) { return t->id() == order_id; }));
    }

    sortPositions();
    return true;
}

const std::vector<const OrderWrapper*>& OrderManager::getPositions() const {
    return _positions;
}

void OrderManager::sortPositions() {
    // sort by price
    std::sort(_positions.begin(), _positions.end(), [](const OrderWrapper* l, const OrderWrapper* r) {
        return l->price() < r->price();
    });
}

void OrderManager::printOrder(const Symbol& symbol, const OrderWrapper* order, const OrderWrapper* position/* = nullptr*/) {
    auto sideStr = [](const OrderSide& side) {
        if (side == OrderSide::Buy) return "BUY";
        if (side == OrderSide::Sell) return "SELL";
        return "INVALID";
    };

    if (position == nullptr) {
        Logger::info("\a%s\t%f %s for\t%f", sideStr(order->side()), order->baseQuantity(), symbol.baseAsset().c_str(), order->price());
    } else {
        Logger::info("\a%s\t%f %s for %f (%s for %f)", sideStr(order->side()), order->baseQuantity(), symbol.baseAsset().c_str(), order->price(), sideStr(position->side()), position->price());

        double profit = std::abs(position->price() - symbol.getPrice()) * position->baseQuantity();
        double total = DataManager::addProfit(symbol.quoteAsset(), profit);
	double current = DataManager::addProfit(symbol, profit);

        std::string formatProfit = "%." + std::to_string(util::zeros_after_dot(profit) + 1) + "f";
        std::string formatSymbolProfit = "%." + std::to_string(util::zeros_after_dot(current) + 1) + "f";
        std::string format = "%sprofit update: +" + formatProfit + " (total +" + formatSymbolProfit + ") %s%s";
        Logger::info(format.c_str(), GREEN, profit, current, symbol.quoteAsset().c_str(), RESET);
    }
}
