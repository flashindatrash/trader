#pragma once

#include "Proxy.hpp"
#include "exchanger/binance/response/BinanceOrderData.hpp"

class Symbol;
struct SideEnum;

class BinanceOrders : public core::Proxy<BinanceOrders>
{
public: // methods
    BinanceOrders() = default;

    std::vector<BinanceOrderData> getAllOrders(const Symbol& symbol, int limit = 0) const;

    bool isEnough(const Symbol& symbol, const SideEnum& side, double quantity) const;

    const BinanceOrderData createOrder(const Symbol& symbol, const SideEnum& side, double quantity) const;
    const BinanceOrderData createOrder(const Symbol& symbol, const SideEnum& side, double quantity, const std::string& type) const;
};

#define SOrders() BinanceOrders::getInstance()
