#pragma once

#include "exchanger/base/Identifier.hpp"
#include "exchanger/base/OrderBase.hpp"

struct Ticker {
    std::string symbol;
    time_t time = 0;
    double bestBidPrice = 0.0;
    double bestBidQty = 0.0;
    double bestAskPrice = 0.0;
    double bestAskQty = 0.0;
};

class PriceWrapper : public Identifier {
public: // static
    static PriceWrapper* create();

public: // methods
    PriceWrapper() = default;

    void set(Price price);
    void set(Ticker ticker);

    const Price& get() const;
    const Price& get(const OrderSide& side) const;

protected: // vars
    Price _price = 0.0;
    Ticker _ticker;
};

