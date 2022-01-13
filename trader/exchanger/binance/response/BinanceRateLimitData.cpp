//
// Created by Вадим Проскурин on 14.01.2022.
//

#include "BinanceRateLimitData.hpp"
#include <json/json.h>

BinanceRateLimitData::BinanceRateLimitData(const Json::Value& json) {
    type = json["rateLimitType"].asString();
    interval = json["interval"].asString();
    intervalNum = atoi(json["intervalNum"].asString().c_str());
    limit = atoi(json["limit"].asString().c_str());
}