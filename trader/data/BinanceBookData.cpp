#include "BinanceBookData.hpp"

#include "json/json.h"

BinanceBookData::BinanceBookData(const Json::Value& json)
{
    updateId = atol(json["u"].asString().c_str());
    symbol = json["s"].asString();
    bestBidPrice = atof(json["b"].asString().c_str());
    bestBidQty = atof(json["B"].asString().c_str());
    bestAskPrice = atof(json["a"].asString().c_str());
    bestAskQty = atof(json["A"].asString().c_str());
}
