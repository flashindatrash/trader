//
// Created by Вадим Проскурин on 12.01.2022.
//

#pragma once

#include "exchanger/base/OrderBase.hpp"

namespace Json {
class Value;
}

struct BinancePriceData {
public: // methods
    BinancePriceData() = default;
    BinancePriceData(const Json::Value& json);

public: // vars
    std::string symbol;
    Price price;

};


