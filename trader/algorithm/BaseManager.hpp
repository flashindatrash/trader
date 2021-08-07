#pragma once

#include <global.hpp>

class OrderManager;
class TradeSymbol;

class BaseManager
{
public: // methods
    BaseManager(OrderManager& orders, time_t interval);

    virtual bool check(const TradeSymbol& symbol);

protected: // vars
    OrderManager& _orders;

private: // vars
    time_t _interval = 0;
};

