//
// Created by Вадим Проскурин on 18.04.2022.
//

#include "BinanceOrderFillData.hpp"
#include <json/json.h>

BinanceOrderFillData::BinanceOrderFillData(const Json::Value& json) {
    price = Decimal::deserialize(json["price"].asString());
    qty = Decimal::deserialize(json["qty"].asString());
    commission = Decimal::deserialize(json["commission"].asString());
    commissionAsset = json["commissionAsset"].asString();
}