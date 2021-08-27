#include "Context.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"

NS_USE

bool Context::isValid() const {
    return candlestick != nullptr;
}

const Price& Context::price() const {
    return candlestick->priceClose();
}
