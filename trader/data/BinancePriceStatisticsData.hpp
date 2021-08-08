#pragma once

#include <global.hpp>

struct BinancePriceStatisticsData
{
public: // methods
    BinancePriceStatisticsData() {}
    BinancePriceStatisticsData(const Json::Value& json);

public: // vars
    std::string symbol = "";
    double priceChange = 0.0;
    double priceChangePercent = 0.0;
    double weightedAvgPrice = 0.0;
    double prevClosePrice = 0.0;
    double lastPrice = 0.0;
    double lastQty = 0.0;
    double bidPrice = 0.0;
    double askPrice = 0.0;
    double openPrice = 0.0;
    double highPrice = 0.0;
    double lowPrice = 0.0;
    double volume = 0.0;
    double quoteVolume = 0.0;
    time_t openTime = 0;
    time_t closeTime = 0;
    int count = 0;
};

