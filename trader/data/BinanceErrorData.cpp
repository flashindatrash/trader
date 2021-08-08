#include "data/BinanceErrorData.hpp"
#include "json/json.h"

const int BinanceErrorData::NEW_ORDER_REJECTED = -2010;
const int BinanceErrorData::INVALID_TIMESTAMP = -1021;

BinanceErrorData::BinanceErrorData(const Json::Value& json)
{
    if (not json.isObject() || not json["code"])
        return;

    code = atoi(json["code"].asCString());
    msg = json["msg"].asString();
}

bool BinanceErrorData::has() const {
    return code != 0;
}
