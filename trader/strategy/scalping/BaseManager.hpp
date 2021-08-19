#pragma once


class Symbol;

namespace scalping {
class OrderManager;

class BaseManager {
public: // methods
    BaseManager(OrderManager& orders);

    virtual bool init(const Symbol& symbol) { return true; }
    virtual void tick(const Symbol& symbol) {}

protected: // vars
    OrderManager& _orders;
};

}
