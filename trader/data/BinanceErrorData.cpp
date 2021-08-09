#include "data/BinanceErrorData.hpp"
#include "json/json.h"

const int BinanceErrorData::NEW_ORDER_REJECTED = -2010;
const int BinanceErrorData::INVALID_TIMESTAMP = -1021;

BinanceErrorData::BinanceErrorData(const Json::Value& json)
{
    if (not json.isObject() || not json["code"] || not json["code"].isInt())
        return;

    code = json["code"].asInt();
    msg = json["msg"].asString();
}

bool BinanceErrorData::has() const {
    return code != 0;
}
