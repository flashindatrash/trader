#pragma once

#include "algorithm/BaseManager.hpp"

struct BinanceOrderData;
class TradeAsset;

class ProfitManager : public BaseManager
{
public: // methods
    ProfitManager(OrderManager& orders);

    bool check(const TradeSymbol& symbol) override;

protected: // methods
    // найти ордер, который можно закрыть по выгодной ценеы
    const BinanceOrderData* findClosableOrder(const TradeSymbol& symbol) const;
};

