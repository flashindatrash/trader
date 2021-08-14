#include "BinancePriceStatisticsData.hpp"
#include <json/json.h>

BinancePriceStatisticsData::BinancePriceStatisticsData(const Json::Value& json)
{
    symbol              = json["symbol"].asString();
    priceChange         = atof(json["priceChange"].asString().c_str());
    priceChangePercent  = atof(json["priceChangePercent"].asString().c_str());
    weightedAvgPrice    = atof(json["weightedAvgPrice"].asString().c_str());
    prevClosePrice      = atof(json["prevClosePrice"].asString().c_str());
    lastPrice           = atof(json["lastPrice"].asString().c_str());
    lastQty             = atof(json["lastQty"].asString().c_str());
    bidPrice            = atof(json["bidPrice"].asString().c_str());
    askPrice            = atof(json["askPrice"].asString().c_str());
    openPrice           = atof(json["openPrice"].asString().c_str());
    highPrice           = atof(json["highPrice"].asString().c_str());
    lowPrice            = atof(json["lowPrice"].asString().c_str());
    volume              = atof(json["volume"].asString().c_str());
    quoteVolume         = atof(json["quoteVolume"].asString().c_str());
    openTime            = json["openTime"].asInt64();
    closeTime           = json["closeTime"].asInt64();
    count               = json["count"].asInt();
}

