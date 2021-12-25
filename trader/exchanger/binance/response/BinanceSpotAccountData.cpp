//
// Created by Вадим Проскурин on 25.12.2021.
//

#include "BinanceSpotAccountData.hpp"
#include <json/json.h>

BinanceSpotAccountData::BinanceSpotAccountData(const Json::Value& json) {
    makerCommission = atof(json["makerCommission"].asString().c_str());
    takerCommission = atof(json["takerCommission"].asString().c_str());
    canTrade = json["canTrade"].asBool();

    if (json["balances"].isArray()) {
        for (const auto &balance: json["balances"]) {
            balances.push_back(BinanceBalanceData(balance, "asset", "free", "locked"));
        }
    }
}