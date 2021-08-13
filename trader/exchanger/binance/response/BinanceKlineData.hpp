#pragma once

#include <global.hpp>

struct BinanceKlineData
{
public: // methods
    BinanceKlineData() = default;
    BinanceKlineData(const Json::Value& json);

    bool isEmpty() const;

    double hl2() const;
    double hlc3() const;
    double ohlc4() const;

public: // vars
    std::string symbol = "";
    time_t timeStart = 0;
    time_t timeClose = 0;
    double priceOpen = 0.0;
    double priceClose = 0.0;
    double priceHigh = 0.0;
    double priceLow = 0.0;
    double baseAssetVolume = 0.0;
    double quoteAssetVolume = 0.0;
    double takerBaseAssetVolume = 0.0;
    double takerQuoteAssetVolume = 0.0;
    bool isClosed = false;
    int tradesCount = 0;
};


/*{
  "e": "kline",     // Event type
  "E": 123456789,   // Event time
  "s": "BNBBTC",    // Symbol
  "k": {
    "t": 123400000, // Kline start time
    "T": 123460000, // Kline close time
    "s": "BNBBTC",  // Symbol
    "i": "1m",      // Interval
    "f": 100,       // First trade ID
    "L": 200,       // Last trade ID
    "o": "0.0010",  // Open price
    "c": "0.0020",  // Close price
    "h": "0.0025",  // High price
    "l": "0.0015",  // Low price
    "v": "1000",    // Base asset volume
    "n": 100,       // Number of trades
    "x": false,     // Is this kline closed?
    "q": "1.0000",  // Quote asset volume
    "V": "500",     // Taker buy base asset volume
    "Q": "0.500",   // Taker buy quote asset volume
    "B": "123456"   // Ignore
  }
}*/

/*{
    "E" : 1628199250695,
    "e" : "kline",
    "k" :
    {
        "B" : "0",
        "L" : 987894894,
        "Q" : "1050530.27508676",
        "T" : 1628199359999,
        "V" : "25.63737200",
        "c" : "40976.73000000",
        "f" : 987893629,
        "h" : "40983.33000000",
        "i" : "3m",
        "l" : "40947.57000000",
        "n" : 1266,
        "o" : "40976.82000000",
        "q" : "2101325.30744169",
        "s" : "BTCUSDT",
        "t" : 1628199180000,
        "v" : "51.28544900",
        "x" : false
    },
    "s" : "BTCUSDT"
}*/

