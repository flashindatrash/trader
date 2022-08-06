//
// Created by Вадим Проскурин on 18.04.2022.
//


#pragma once

#include "exchanger/base/OrderBase.hpp"

namespace Json {
class Value;
}

struct BinanceOrderFillData {
    BinanceOrderFillData() = default;
    BinanceOrderFillData(const Json::Value& json);

    Price price;
    Quantity qty;
    Quantity commission;
    std::string commissionAsset;
};


/*
"price": "4000.00000000",
"qty": "1.00000000",
"commission": "4.00000000",
"commissionAsset": "USDT"
*/