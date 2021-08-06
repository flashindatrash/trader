#include "data/BinanceErrorData.hpp"
#include "json/json.h"

const int BinanceErrorData::ERROR_CODE_INSUFFICIENT_BALANCE = -2010;

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
