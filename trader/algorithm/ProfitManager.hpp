#pragma once

#include "algorithm/BaseManager.hpp"

struct BinanceOrderData;

class ProfitManager : public BaseManager
{
public: // methods
    ProfitManager(OrderManager& orders);

    void tick(const Symbol& symbol) override;

protected: // methods
    // найти ордер, который можно закрыть по выгодной ценеы
    const BinanceOrderData* findClosableOrder(const Symbol& symbol) const;
};

