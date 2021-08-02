#include "BinanceOrders.hpp"

#include "binacpp.h"

#include "BinanceConfig.hpp"

BinanceOrders::BinanceOrders() {
}

std::vector<BinanceOrderData> BinanceOrders::getAllOrders(const std::string& symbol, int limit/* = 0*/) const {
    Json::Value result;
    BinaCPP::get_allOrders(symbol.c_str(), 0, limit, BINANCE_RECV_WINDOW, result);

    std::vector<BinanceOrderData> vec;
    if (!result.isArray())
        return vec;

    for (uint i = 0; i < result.size(); ++i)
        vec.push_back(result[i]);

    return vec;
}

const BinanceOrderData BinanceOrders::createOrder(const std::string& symbol, const std::string& side, double quantity) const {
    Json::Value result;
    BinaCPP::send_order(symbol.c_str(), side.c_str(), "MARKET", "GTC", quantity , 0, "", 0, 0, BINANCE_TEST_MODE, BINANCE_RECV_WINDOW, result);
    return BinanceOrderData(result);
}
