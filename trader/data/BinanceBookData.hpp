#pragma once

#include <string>

namespace Json {
    class Value;
}

struct BinanceBookData
{
public: // methods
    BinanceBookData(const Json::Value& json);

public: // vars
    long updateId = 0;
    std::string symbol = "";
    double bestBidPrice = 0.0;
    double bestBidQty = 0.0;
    double bestAskPrice = 0.0;
    double bestAskQty = 0.0;
};

