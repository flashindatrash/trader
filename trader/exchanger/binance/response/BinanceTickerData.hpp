#pragma once

#include "exchanger/wrapper/PriceWrapper.hpp"

namespace Json {
class Value;
}

struct BinanceTickerData : public Ticker {
public: // methods
    BinanceTickerData() = default;
    BinanceTickerData(const Json::Value& json);

public: // vars
    long updateId = 0;
};

