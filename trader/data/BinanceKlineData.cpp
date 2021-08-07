#include "data/BinanceKlineData.hpp"

#include <json/json.h>

BinanceKlineData::BinanceKlineData(const Json::Value& json)
{
    bool is_array = json.isArray();
    const Json::Value& kline = is_array ? json : json["k"];

    if (not is_array)
        symbol = kline["s"].asString();

    timeStart               = (is_array ? kline[0] : kline["t"]).asInt64();
    timeClose               = (is_array ? kline[6] : kline["T"]).asInt64();
    priceOpen               = atof((is_array ? kline[1] : kline["o"]).asString().c_str());
    priceClose              = atof((is_array ? kline[4] : kline["c"]).asString().c_str());
    priceHigh               = atof((is_array ? kline[2] : kline["h"]).asString().c_str());
    priceLow                = atof((is_array ? kline[3] : kline["l"]).asString().c_str());
    baseAssetVolume         = atof((is_array ? kline[5] : kline["v"]).asString().c_str());
    quoteAssetVolume        = atof((is_array ? kline[7] : kline["q"]).asString().c_str());
    takerBaseAssetVolume    = atof((is_array ? kline[9] : kline["V"]).asString().c_str());
    takerQuoteAssetVolume   = atof((is_array ? kline[10] : kline["Q"]).asString().c_str());
    isClosed                = (is_array ? true : kline["x"]).asBool();
    tradesCount             = (is_array ? kline[8] : kline["n"]).asInt();
};

bool BinanceKlineData::isEmpty() const {
    return timeStart == 0 || timeClose == 0;
}

double BinanceKlineData::hl2() const {
    return (priceHigh + priceLow) / 2.0;
}

double BinanceKlineData::hlc3() const {
    return (priceHigh + priceLow + priceClose) / 3.0;
}

double BinanceKlineData::ohlc4() const {
    return (priceOpen + priceHigh + priceLow + priceClose) / 4.0;
}
