#pragma once

#include <string>

namespace Json {
class Value;
}

struct BinanceBalanceData {
public: // methods
    BinanceBalanceData() = default;
    BinanceBalanceData(std::string a, double f);
    BinanceBalanceData(const Json::Value& json, const std::string& asset_key, const std::string& amount_key, const std::string& locked_key = "");

public: // vars
    std::string asset = "";
    double free = 0.0;
    double locked = 0.0;
};

