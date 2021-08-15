#pragma once

#include <string>
#include "exchanger/wrapper/CandlestickWrapper.hpp"

namespace Json {
    class Value;
}

struct BinancePriceStatisticsData : public Candlestick {
public: // methods
    BinancePriceStatisticsData() = default;
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
    double volume = 0.0;
    double quoteVolume = 0.0;
    int count = 0;
};

