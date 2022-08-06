#pragma once

#include "exchanger/base/Identifier.hpp"
#include "exchanger/base/OrderBase.hpp"

struct Candlestick {
    std::string symbol;
    Price price_open, price_high, price_low, price_close;
    time_t time_open, time_close;
    bool closed = false;
};

class CandlestickWrapper : public Identifier {
public: // static
    static CandlestickWrapper* create();

public: // methods
    void set(const Candlestick& data);

    const Price& priceOpen() const;
    const Price& priceClose() const;
    Price priceMin() const;
    Price priceMax() const;

    Price bodyLen() const;
    Price wickLen() const; // upper tail
    Price tailLen() const; // lowest tail

    bool isBullish() const; // green
    bool isBearish() const; // red

    Price hl2() const;
    Price hlc3() const;
    Price ohlc4() const;

    const time_t& timeOpen() const;
    const time_t& timeClose() const;

    const bool& isClosed() const;
    void close();

protected: // methods
    CandlestickWrapper() = default;

protected: // vars
    Candlestick _data;
};

