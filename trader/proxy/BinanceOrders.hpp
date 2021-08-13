#pragma once

#include "Proxy.hpp"
#include "exchanger/binance/response/BinanceOrderData.hpp"

class TradeSymbol;

class BinanceOrders : public core::Proxy<BinanceOrders>
{
public: // methods
    BinanceOrders() = default;

    std::vector<BinanceOrderData> getAllOrders(const TradeSymbol& symbol, int limit = 0) const;

    bool isEnough(const TradeSymbol& symbol, const BinanceSideEnum& side, double quantity) const;

    const BinanceOrderData createOrder(const TradeSymbol& symbol, const BinanceSideEnum& side, double quantity) const;
    const BinanceOrderData createOrder(const TradeSymbol& symbol, const BinanceSideEnum& side, double quantity, const std::string& type) const;
};

#define SOrders() BinanceOrders::getInstance()
