//
// Created by Вадим Проскурин on 14.01.2022.
//


#pragma once

#include "BinanceRateLimitData.hpp"
#include "BinanceSymbolData.hpp"
#include <unordered_map>

struct BinanceExchangeData {
public: // methods
    BinanceExchangeData() = default;
    BinanceExchangeData(const Json::Value& json);

public: // vars
    std::vector<BinanceRateLimitData> rateLimits;
    std::unordered_map<std::string, BinanceSymbolData> symbols;
};


