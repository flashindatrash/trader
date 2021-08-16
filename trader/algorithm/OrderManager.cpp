#include "Logger.hpp"
#include "proxy/TraderTime.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/base/ExchangerTypes.hpp"
#include "exchanger/base/Symbol.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/DataManager.hpp"

OrderManager::OrderManager(const Symbol& symbol, bool test_mode)
    : _test_mode(test_mode)
{
    if (_test_mode)
        Logger::info("TEST MODE!");

    // найдем все открытые позиции
    const std::vector<const OrderWrapper*>& orders = Exchanger().book()->get();
    std::vector<std::string> keys = DataManager::getPositionIds();
    for (const OrderWrapper* order : orders) {
        std::string id = DataManager::sDbKeyOrder + order->getId();
        if (std::find(keys.begin(), keys.end(), id) == keys.end())
            continue;

        _positions.push_back(order);
    }
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

    const Symbol& symbol = Exchanger().book()->getIdentifier();

    // открыть/закрыть транзакцию
    if (transaction == nullptr) {
        Logger::info("\a%s %f %s for %f", request.side.c_str(), request.quantity, symbol.baseAsset().c_str(), symbol.getPrice());
        DataManager::openPosition(result->getId());
        _positions.push_back(result);
    } else {
        Logger::info("\a%s %f %s for %f (%s for %f)", request.side.c_str(), request.quantity, symbol.baseAsset().c_str(), symbol.getPrice(), transaction->side().c_str(), transaction->getPrice());
        double profit = std::abs(transaction->getPrice() - symbol.getPrice()) * transaction->quantity();
        printProfit(symbol, profit);
        std::string order_id = transaction->getId();
        DataManager::closePosition(order_id);
        _positions.erase(std::remove_if(_positions.begin(), _positions.end(), [order_id](const OrderWrapper* t) { return t->getId() == order_id; }));
    }

    return true;
}

const std::vector<const OrderWrapper*>& OrderManager::getPositions() const {
    return _positions;
}

void OrderManager::printProfit(const Symbol& symbol, double profit) {
    double profit_total = DataManager::addProfit(symbol.quoteAsset(), profit);
    double losses_total = 0.0;

    double current_price = symbol.getPrice();
    for (const OrderWrapper* position : _positions) {
        double order_price = position->getPrice();

        bool sell_loss = SideEnum(position->side()) == SideEnum::Sell && current_price > order_price;
        bool buy_loss = SideEnum(position->side()) == SideEnum::Buy && current_price < order_price;

        if (sell_loss || buy_loss)
            losses_total += std::abs(current_price - order_price) * position->quantity();
    }

    Logger::info("%sprofit update: +%.4f (total +%.4f / loss -%.4f) %s%s", GREEN, profit, profit_total, losses_total, symbol.quoteAsset().c_str(), RESET);
}
