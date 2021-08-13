#pragma once

#include "algorithm/BaseManager.hpp"

class BinanceSideEnum;
class CandlestickContainer;
class BinanceKlineData;

class TraderManager : public BaseManager
{
public: // methods
    TraderManager(OrderManager& orders);

    bool check(const Symbol& symbol) override;

protected: // methods
    // найти схожую транзакцию по цене
    bool hasEqualPosition(const BinanceSideEnum& side, double price) const;
    // свеча закрылась
    void onCloseCandle(const BinanceKlineData& data);

protected: // vars
    double _min_quantity = 0.0;
    CandlestickContainer* _candlesticks = nullptr;
};

