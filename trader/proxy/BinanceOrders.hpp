#pragma once

#include "Proxy.hpp"
#include "data/BinanceOrderData.hpp"

class BinanceOrders : public Core::Proxy<BinanceOrders>
{
public: // methods
    BinanceOrders();

    // get all orders
    std::vector<BinanceOrderData> getAllOrders(const std::string& symbol, int limit = 0) const;
    // create order
    const BinanceOrderData createOrder(const std::string& symbol, const std::string& side, double quantity) const;
};

#define SBinanceOrders() BinanceOrders::getInstance()
