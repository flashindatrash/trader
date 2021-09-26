#include "Context.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"

NS_USE

Context::Context(const CandlestickWrapper* candlestick)
    : candlestick(candlestick)
{
}

const Price& Context::price(const OrderSide& side) const {
    return Exchanger().price(candlestick->id())->get(side);
}