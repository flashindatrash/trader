//
// Created by Вадим Проскурин on 24.12.2021.
//

#pragma once

#include "BinanceBalanceData.hpp"

class BinanceFlexibleProductData : public BinanceBalanceData {
public: // methods
    BinanceFlexibleProductData() = default;
    BinanceFlexibleProductData(const Json::Value& json);

public: // vars
    std::string productId = "";
    bool canRedeem = false;
    double redeemingAmount = 0.0;
};


