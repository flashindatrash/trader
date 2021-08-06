#include "Logger.hpp"
#include "proxy/BinanceOrders.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/Database.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "algorithm/OrderManager.hpp"

OrderManager::OrderManager(const TradeSymbol& symbol)
{
    _orders = SOrders().getAllOrders(symbol);

    // найдем все открытые транзакции
    std::vector<std::string> keys = DB().keys("order:*");
    for (const BinanceOrderData& order : _orders) {
        std::string id = key(order);
        if (std::find(keys.begin(), keys.end(), id) == keys.end())
            continue;

        _transactions.push_back(order);
        trace("- %s %f\n", order.side.c_str(), order.getPrice());
    }
}

bool OrderManager::create(const TradeSymbol& symbol, const std::string& side, double quantity, bool transaction) {
    // проверяем, что достаточно средств
    if (not SOrders().isEnough(symbol, side, quantity))
        return false;

    BinanceOrderData result = SOrders().createOrder(symbol, side, quantity);

    // обновляем время даже если не смогли создать заказ, мб после таймаута исправится
    _last_time = STime().getCurrent();

    // что-то случилось, не удалось создать заказ
    if (result.isEmpty() || result.isRejected())
        return false;

    // пишем в лог
    trace("%s %s at price %f\n", side.c_str(), symbol.baseAsset().c_str(), symbol.getPrice());

    // сохранить историю
    _orders.push_back(result);

    // открыть транзакцию
    if (transaction)
        open(result);

    return true;
}

void OrderManager::open(const BinanceOrderData& transaction) {
    DB().set(key(transaction), true);
    _transactions.push_back(transaction);
}

void OrderManager::close(const BinanceOrderData& transaction) {
    std::string id = key(transaction);
    DB().del(id);
    std::remove_if(_transactions.begin(), _transactions.end(), [id](BinanceOrderData t) { return key(t) == id; });
}

const std::vector<BinanceOrderData>& OrderManager::getOrders() const {
    return _orders;
}

const std::vector<BinanceOrderData>& OrderManager::getTransactions() const {
    return _transactions;
}

const std::string OrderManager::key(const BinanceOrderData& transaction) {
    return "order:" + transaction.clientOrderId;
}

time_t OrderManager::getLastTime() const {
    return _last_time;
}

