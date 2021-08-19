#pragma once

#include "BaseManager.hpp"

class OrderWrapper;
enum OrderSide : unsigned int;

namespace scalping {

class ProfitManager : public BaseManager {
public: // methods
    ProfitManager(OrderManager& orders);

    void tick(const Symbol& symbol) override;

protected: // methods
    // найти ордер, который можно закрыть по выгодной ценеы
    const OrderWrapper* findClosableOrder(const Symbol& symbol) const;
    // развернуть тип сделки
    OrderSide revertSide(const OrderSide& original) const;

protected: // vars
    double _temp = 0.0;
};

}
