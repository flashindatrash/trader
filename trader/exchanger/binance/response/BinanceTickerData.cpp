#include "BinanceTickerData.hpp"
#include <json/json.h>

BinanceTickerData::BinanceTickerData(const Json::Value& json) {
    updateId = atol(json["u"].asString().c_str());
    symbol = json["s"].asString();
    bestBidPrice = Decimal::deserialize(json["b"].asString());
    bestBidQty = Decimal::deserialize(json["B"].asString());
    bestAskPrice = Decimal::deserialize(json["a"].asString());
    bestAskQty = Decimal::deserialize(json["A"].asString());
}
