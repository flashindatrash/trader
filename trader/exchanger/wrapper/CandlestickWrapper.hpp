#pragma once

#include "exchanger/base/Identifier.hpp"
#include "exchanger/base/OrderBase.hpp"

struct Candlestick {
    std::string symbol;
    Price price_open, price_high, price_low, price_close = 0.0;
    time_t time_open, time_close = 0;
    bool closed = false;
};

class CandlestickWrapper : public Identifier {
public: // static
    static CandlestickWrapper* create();

public: // methods
    void set(Candlestick data);

    const Price& priceOpen() const;
    const Price& priceClose() const;

    const Price bodyLen() const;
    const Price wickLen() const; // upper tail
    const Price tailLen() const; // lowest tail

    bool isBullish() const; // green
    bool isBearish() const; // red

    const Price hl2() const;
    const Price hlc3() const;
    const Price ohlc4() const;

    const time_t& timeOpen() const;
    const time_t& timeClose() const;

    const bool& isClosed() const;
    void close();

protected: // methods
    CandlestickWrapper() = default;

protected: // vars
    Candlestick _data;
};

