#pragma once

#include "algorithm/BaseManager.hpp"

class BinanceSideEnum;

class TraderManager : public BaseManager
{
public: // methods
    TraderManager(OrderManager& orders);

    bool check(const TradeSymbol& symbol) override;

protected: // methods
    // найти схожую транзакцию по цене
    bool hasEqualTransaction(const BinanceSideEnum& side, double price) const;

protected: // vars
    double _min_quantity = 0.0;
};

