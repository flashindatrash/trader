//
// Created by Вадим Проскурин on 12.01.2022.
//

#include "BinancePriceData.hpp"
#include <json/json.h>

BinancePriceData::BinancePriceData(const Json::Value& json) {
    symbol = json["symbol"].asString();
    price = Decimal::deserialize(json["price"].asString());
}