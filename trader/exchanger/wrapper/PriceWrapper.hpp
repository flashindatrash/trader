#pragma once

#include "exchanger/base/Identifier.hpp"
#include "exchanger/base/OrderBase.hpp"

struct Ticker {
    std::string symbol;
    time_t time = 0;
    Decimal bestBidPrice;
    Decimal bestBidQty;
    Decimal bestAskPrice;
    Decimal bestAskQty;
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

    const Ticker& ticker() const;

protected: // vars
    Price _price;
    Ticker _ticker;
};

