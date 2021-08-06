#pragma once

#include <global.hpp>

class OrderManager;
class TradeSymbol;

class BaseManager
{
public: // methods
    BaseManager(OrderManager& orders);

    virtual bool check(const TradeSymbol& symbol);

protected: // vars
    OrderManager& _orders;
    time_t _interval = 0;
};

