//
// Created by Вадим Проскурин on 24.12.2021.
//

#include "BinanceFlexibleProductData.hpp"
#include <json/json.h>

BinanceFlexibleProductData::BinanceFlexibleProductData(const Json::Value& json)
    : BinanceBalanceData(json, "asset", "freeAmount", "lockedAmount")
{
    productId = json["productId"].asString();
    canRedeem = json["canRedeem"].asBool();
    redeemingAmount = atof(json["redeemingAmount"].asString().c_str());
}