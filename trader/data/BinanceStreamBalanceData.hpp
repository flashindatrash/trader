#pragma once

#include <string>

namespace Json {
    class Value;
}

struct BinanceStreamBalanceData
{
public: // methods
    BinanceStreamBalanceData(const Json::Value& json);

public: // vars
    std::string asset = "";
    double free = 0.0;
    double locked = 0.0;
};

/*
{
    "a" : "BTC",
    "f" : "0.00035313",
    "l" : "0.00000000"
}
*/
