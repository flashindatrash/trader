#pragma once

#include "exchanger/wrapper/ChartWrapper.hpp"
#include "Defines.hpp"

class CandlestickWrapper;

NS_BEGIN
class Context {
public: // methods
    Context(ChartWrapper::ConstIterator it, const PriceWrapper& price);

    const CandlestickWrapper& candlestick() const;
    const std::string& id() const;
    const Price& price() const;
    const Price& price(const OrderSide& side) const;
    Price ema(size_t length) const;

protected: // vars
    ChartWrapper::ConstIterator _it;
    const PriceWrapper& _price;
};
NS_END

