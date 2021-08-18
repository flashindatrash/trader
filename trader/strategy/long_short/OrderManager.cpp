#include "OrderManager.hpp"
#include "DataManager.hpp"
#include "Logger.hpp"
#include "proxy/Time.hpp"
#include "proxy/Exchanger.hpp"
#include "exchanger/base/ExchangerTypes.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/BookWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "util/NumberUtil.hpp"

OrderManager::OrderManager(const Symbol& symbol, bool test_mode)
    : _test_mode(test_mode)
{
    if (_test_mode)
        Logger::info("TEST MODE!");

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
        printOrder(order);
    Logger::info("======");
}

bool OrderManager::create(const OrderRequest& request, const OrderWrapper* transaction) {
    if (_test_mode)
        return false;

    // проверяем, что достаточно средств
    if (not request.isEnough())
        return false;

    const OrderWrapper* result = Exchanger().createOrder(request);
    if (result == nullptr)
        return false;


    printOrder(result, transaction);

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

void OrderManager::printOrder(const OrderWrapper* order, const OrderWrapper* position/* = nullptr*/) {
    const Symbol& symbol = Exchanger().book()->id();

    auto sideStr = [](const OrderSide& side) {
        if (side == OrderSide::Buy) return "BUY";
        if (side == OrderSide::Sell) return "SELL";
        return "INVALID";
    };

    if (position == nullptr) {
        Logger::info("\a%s\t%f %s for\t%f", sideStr(order->side()), order->quantity(), symbol.baseAsset().c_str(), order->price());
    } else {
        Logger::info("\a%s\t%f %s for %f (%s for %f)", sideStr(order->side()), order->quantity(), symbol.baseAsset().c_str(), order->price(), sideStr(position->side()), position->price());

        double profit = std::abs(position->price() - symbol.getPrice()) * position->quantity();
        double total = DataManager::addProfit(symbol.quoteAsset(), profit);
        std::string formatCurrentProfit = "%." + std::to_string(util::zeros_after_dot(profit) + 1) + "f";
        std::string formatTotalProfit = "%." + std::to_string(util::zeros_after_dot(total) + 1) + "f";
        std::string format = "%sprofit update: +" + formatCurrentProfit + " (total +" + formatTotalProfit + ") %s%s";
        Logger::info(format.c_str(), GREEN, profit, total, symbol.quoteAsset().c_str(), RESET);
    }
}
