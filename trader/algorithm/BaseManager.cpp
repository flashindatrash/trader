#include "proxy/BinanceTime.hpp"
#include "algorithm/OrderManager.hpp"
#include "algorithm/BaseManager.hpp"

BaseManager::BaseManager(OrderManager& orders)
    : _orders(orders)
{
}

bool BaseManager::check(const TradeSymbol& symbol) {
    if (STime().getCurrent() < _orders.getLastTime() + _interval)
        return false;

    return true;
}
