#pragma once

#include "exchanger/base/Storage.hpp"
#include "exchanger/base/ExchangerTypes.hpp"

struct Candlestick {
    Price price_open, price_high, price_low, price_close = 0.0;
    time_t time_open, time_close = 0;
    bool closed = false;
};

class CandlestickWrapper : public MapIdentifier<std::string> {
public: // static
    static CandlestickWrapper* create();

public: // methods
    void set(Candlestick data);

    const Price& priceOpen() const;
    const Price& priceClose() const;

    const Price bodyLen() const;
    const Price wickLen() const;
    const Price tailLen() const;

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

