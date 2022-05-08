//
// Created by Вадим Проскурин on 18.04.2022.
//

#include "BinanceOrderFillData.hpp"
#include <json/json.h>

BinanceOrderFillData::BinanceOrderFillData(const Json::Value& json) {
    price = atof(json["price"].asString().c_str());
    qty = atof(json["qty"].asString().c_str());
    commission = atof(json["commission"].asString().c_str());
    commissionAsset = json["commissionAsset"].asString();
}