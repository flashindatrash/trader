//
// Created by Вадим Проскурин on 12.01.2022.
//

#include "BinancePriceData.hpp"
#include <json/json.h>

BinancePriceData::BinancePriceData(const Json::Value& json) {
    symbol = json["symbol"].asString();
    price = atof(json["price"].asString().c_str());
}