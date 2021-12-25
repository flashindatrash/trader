//
// Created by Вадим Проскурин on 25.12.2021.
//

#pragma once

#include "BinanceBalanceData.hpp"
#include <vector>

struct BinanceSpotAccountData {
public: // methods
    BinanceSpotAccountData() = default;
    BinanceSpotAccountData(const Json::Value& json);

public: // vars
    double makerCommission = 0.0;
    double takerCommission = 0.0;
    bool canTrade = false;
    std::vector<BinanceBalanceData> balances;
};


