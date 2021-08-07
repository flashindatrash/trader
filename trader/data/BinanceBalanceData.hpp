#pragma once

#include <global.hpp>

struct BinanceBalanceData
{
public: // methods
    BinanceBalanceData() {}
    BinanceBalanceData(std::string a, double f);
    BinanceBalanceData(const Json::Value& json, bool minimized);

public: // vars
    std::string asset = "";
    double free = 0.0;
    double locked = 0.0;
};

