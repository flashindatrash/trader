#pragma once

#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/indicator/EMACross.hpp"
#include "Defines.hpp"

class CandlestickWrapper;

NS_BEGIN
class Context {
public: // methods
    Context(ChartWrapper::ConstIterator it, const PriceWrapper& price);

    const CandlestickWrapper& candlestick() const;
    const std::string& id() const;
    time_t time() const;
    const Price& price() const;
    const Price& price(const OrderSide& side) const;
    EMACross ema(size_t long_length, size_t short_length) const;

protected: // vars
    ChartWrapper::ConstIterator _it;
    const PriceWrapper& _price;
};
NS_END

