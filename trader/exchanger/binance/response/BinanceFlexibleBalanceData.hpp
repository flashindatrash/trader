//
// Created by Вадим Проскурин on 24.12.2021.
//

#pragma once

#include "BinanceBalanceData.hpp"

struct BinanceFlexibleBalanceData : public BinanceBalanceData {
public: // methods
    BinanceFlexibleBalanceData() = default;
    BinanceFlexibleBalanceData(const Json::Value& json);

public: // vars
    std::string productId = "";
    bool canRedeem = false;
    double redeemingAmount = 0.0;
};


