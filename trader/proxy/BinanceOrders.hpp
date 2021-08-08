#pragma once

#include "Proxy.hpp"
#include "data/BinanceOrderData.hpp"

class TradeSymbol;

class BinanceOrders : public Core::Proxy<BinanceOrders>
{
public: // methods
    BinanceOrders() {}

    std::vector<BinanceOrderData> getAllOrders(const TradeSymbol& symbol, int limit = 0) const;

    bool isEnough(const TradeSymbol& symbol, const BinanceSideEnum& side, double quantity) const;

    const BinanceOrderData createOrder(const TradeSymbol& symbol, const BinanceSideEnum& side, double quantity) const;
    const BinanceOrderData createOrder(const TradeSymbol& symbol, const BinanceSideEnum& side, double quantity, const std::string& type) const;
};

#define SOrders() BinanceOrders::getInstance()
