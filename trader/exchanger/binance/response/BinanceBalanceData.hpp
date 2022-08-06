#pragma once

#include "exchanger/base/Decimal.hpp"

namespace Json {
class Value;
}

struct BinanceBalanceData {
public: // methods
    BinanceBalanceData() = default;
    BinanceBalanceData(const Json::Value& json, const std::string& asset_key, const std::string& amount_key, const std::string& locked_key = "");

public: // vars
    std::string asset;
    Decimal free;
    Decimal locked;
};

