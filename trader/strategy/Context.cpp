#include "Context.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"

NS_USE

Context::Context(ChartWrapper::ConstIterator it, const PriceWrapper& price)
    : _it(it)
    , _price(price)
{
}

const CandlestickWrapper& Context::candlestick() const {
    return **_it;
}

const std::string& Context::id() const {
    return candlestick().id();
}

const Price& Context::price() const {
    return _price.get();
}

const Price& Context::price(const OrderSide& side) const {
    return _price.get(side);
}

Price Context::ema(size_t length) const {
    const ChartWrapper* chart = Exchanger().chart(id());
    if (chart == nullptr)
        return 0.0;

    return chart->ema(_it, length);
}