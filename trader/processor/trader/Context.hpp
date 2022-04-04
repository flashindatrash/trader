#pragma once

#include "exchanger/wrapper/ChartWrapper.hpp"

class CandlestickWrapper;
class PriceWrapper;
class BaseIndicator;

namespace trader {
class Context {
public: // static
    static const Context* current;

public: // methods
    Context(ChartWrapper::ConstIterator it, const PriceWrapper& price);

    const CandlestickWrapper& candlestick() const;
    const std::string& id() const;
    time_t time() const;
    const Price& price() const;
    const Price& price(const OrderSide& side) const;
    bool load(BaseIndicator& indicator) const;

protected: // vars
    ChartWrapper::ConstIterator _it;
    const PriceWrapper& _price;
};
}

