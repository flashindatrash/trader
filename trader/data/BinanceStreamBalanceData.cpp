#include "BinanceStreamBalanceData.hpp"

#include <json/json.h>

BinanceStreamBalanceData::BinanceStreamBalanceData(const Json::Value& json)
{
    asset = json["a"].asString();
    free = atof(json["f"].asString().c_str());
    locked = atof(json["l"].asString().c_str());
}
