//
// Created by Вадим Проскурин on 14.01.2022.
//

#pragma once

#include <string>

namespace Json {
    class Value;
}

struct BinanceRateLimitData {
public: // methods
    BinanceRateLimitData() = default;
    BinanceRateLimitData(const Json::Value& json);

public: // vars
    std::string type = "";
    std::string interval = "";
    int intervalNum = 0;
    int limit = 0;
};


