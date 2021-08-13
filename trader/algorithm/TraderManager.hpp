#pragma once

#include "algorithm/BaseManager.hpp"

class BinanceSideEnum;
class KlineHistory;
class BinanceKlineData;

class TraderManager : public BaseManager
{
public: // methods
    TraderManager(OrderManager& orders);

    bool check(const TradeSymbol& symbol) override;

protected: // methods
    // найти схожую транзакцию по цене
    bool hasEqualPosition(const BinanceSideEnum& side, double price) const;
    // свеча закрылась
    void onCloseCandle(const BinanceKlineData& data);

protected: // vars
    double _min_quantity = 0.0;
    KlineHistory* _candlesticks = nullptr;
};

