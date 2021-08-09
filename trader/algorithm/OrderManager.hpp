#pragma once

#include "data/BinanceOrderData.hpp"

class TradeSymbol;

class OrderManager
{
protected: // static
    static const std::string key(const BinanceOrderData& transaction);

public: // methods
    OrderManager(const TradeSymbol& symbol);

    bool create(const TradeSymbol& symbol, const BinanceSideEnum& side, double quantity, const BinanceOrderData* transaction);

    const std::vector<BinanceOrderData>& getOrders() const;
    const std::vector<BinanceOrderData>& getPositions() const;

    time_t getLastTime() const;

protected: // methods
    void printPositionsTimeline(double current);

protected: // vars
    std::vector<BinanceOrderData> _orders;
    std::vector<BinanceOrderData> _positions;
    time_t _last_time = 0;
};

