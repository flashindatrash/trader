#pragma once

#include "Defines.hpp"

class CandlestickWrapper;

NS_BEGIN
struct Context {
    Context(const CandlestickWrapper* candlestick);

    const CandlestickWrapper* candlestick = nullptr;

    const Price& price(const OrderSide& side) const;

    double risk(const OrderSide& side) const;
};
NS_END

