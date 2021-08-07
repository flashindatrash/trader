#include "data/BinanceErrorData.hpp"
#include "json/json.h"

const int BinanceErrorData::NEW_ORDER_REJECTED = -2010;
const int BinanceErrorData::INVALID_TIMESTAMP = -1021;

BinanceErrorData::BinanceErrorData(const Json::Value& json)
{
    if (not json.isObject())
        return;

    code = atoi(json["code"].asString().c_str());
    msg = json["msg"].asString();
}

bool BinanceErrorData::has() const {
    return code != 0;
}
