#pragma once

#include "algorithm/BaseManager.hpp"

struct BinanceOrderData;

class ProfitManager : public BaseManager
{
public: // methods
    ProfitManager(OrderManager& orders);

    bool check(const TradeSymbol& symbol) override;
};

