#include "proxy/BinanceTime.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/BaseManager.hpp"

bool BaseManager::sDebug = true;

BaseManager::BaseManager(OrderManager& orders, time_t interval)
    : _orders(orders)
    , _interval(interval)
{
}

bool BaseManager::check(const TradeSymbol& symbol) {
    if (STime().getCurrent() < _orders.getLastTime() + _interval)
        return false;

    return true;
}
