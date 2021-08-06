#pragma once

#include "algorithm/BaseManager.hpp"

class TraderManager : public BaseManager
{
public: // methods
    TraderManager(OrderManager& orders);

    bool check(const TradeSymbol& symbol) override;

protected: // vars
    double _min_quantity = 0.0;
};

