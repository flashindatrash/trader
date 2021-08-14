#pragma once

#include "exchanger/binance/response/BinanceOrderData.hpp"

class Symbol;

class OrderManager
{
protected: // static
    static const std::string key(const BinanceOrderData& transaction);

public: // methods
    OrderManager(const Symbol& symbol);

    bool create(const Symbol& symbol, const SideEnum& side, double quantity, const BinanceOrderData* transaction);

    const std::vector<BinanceOrderData>& getOrders() const;
    const std::vector<BinanceOrderData>& getPositions() const;

protected: // methods
    void printProfit(const Symbol& symbol, double profit);
    void printPositionsTimeline(double current);

protected: // vars
    std::vector<BinanceOrderData> _orders;
    std::vector<BinanceOrderData> _positions;
};

