#pragma once

#include <global.hpp>

struct BinanceBalanceData
{
public: // methods
    BinanceBalanceData() {}
    BinanceBalanceData(const Json::Value& json, bool minimized);

public: // vars
    std::string asset = "";
    double free = 0.0;
    double locked = 0.0;
};

