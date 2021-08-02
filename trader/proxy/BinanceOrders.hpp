#pragma once

#include "Proxy.hpp"
#include "data/BinanceOrderData.hpp"

class BinanceSymbol;

class BinanceOrders : public Core::Proxy<BinanceOrders>
{
public: // methods
    BinanceOrders() {}

    std::vector<BinanceOrderData> getAllOrders(const BinanceSymbol& symbol, int limit = 0) const;
    const BinanceOrderData createOrder(const BinanceSymbol& symbol, const std::string& side, double quantity) const;

protected: // vars
    BinanceOrderData _last_order;
};

#define SBinanceOrders() BinanceOrders::getInstance()
