#pragma once

#include "algorithm/BaseManager.hpp"

class OrderWrapper;

class ProfitManager : public BaseManager
{
public: // methods
    ProfitManager(OrderManager& orders);

    void tick(const Symbol& symbol) override;

protected: // methods
    // найти ордер, который можно закрыть по выгодной ценеы
    const OrderWrapper* findClosableOrder(const Symbol& symbol) const;
};

