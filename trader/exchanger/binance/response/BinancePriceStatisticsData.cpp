#include "json/json.h"
#include "exchanger/binance/response/BinancePriceStatisticsData.hpp"

BinancePriceStatisticsData::BinancePriceStatisticsData(const Json::Value& json)
{
    symbol              = json["symbol"].asString();
    priceChange         = atof(json["priceChange"].asCString());
    priceChangePercent  = atof(json["priceChangePercent"].asCString());
    weightedAvgPrice    = atof(json["weightedAvgPrice"].asCString());
    prevClosePrice      = atof(json["prevClosePrice"].asCString());
    lastPrice           = atof(json["lastPrice"].asCString());
    lastQty             = atof(json["lastQty"].asCString());
    bidPrice            = atof(json["bidPrice"].asCString());
    askPrice            = atof(json["askPrice"].asCString());
    openPrice           = atof(json["openPrice"].asCString());
    highPrice           = atof(json["highPrice"].asCString());
    lowPrice            = atof(json["lowPrice"].asCString());
    volume              = atof(json["volume"].asCString());
    quoteVolume         = atof(json["quoteVolume"].asCString());
    openTime            = json["openTime"].asInt64();
    closeTime           = json["closeTime"].asInt64();
    count               = json["count"].asInt();
}

