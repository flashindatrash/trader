#pragma once

#include <global.hpp>

class OrderManager;
class Symbol;

class BaseManager
{
protected: // static
    static bool sDebug;

public: // methods
    BaseManager(OrderManager& orders, time_t interval);

    virtual bool check(const Symbol& symbol);

protected: // vars
    OrderManager& _orders;

private: // vars
    time_t _interval = 0;
};

