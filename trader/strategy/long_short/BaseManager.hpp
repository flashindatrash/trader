#pragma once

class OrderManager;
class Symbol;

class BaseManager
{
public: // methods
    BaseManager(OrderManager& orders);

    virtual bool init(const Symbol& symbol) { return true; }
    virtual void tick(const Symbol& symbol) {}

protected: // vars
    OrderManager& _orders;
};

