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

time_t Context::time() const {
    return candlestick().timeClose();
}

const Price& Context::price() const {
    return _price.get();
}

const Price& Context::price(const OrderSide& side) const {
    return _price.get(side);
}

EMACross Context::ema(size_t long_length, size_t short_length) const {
    const ChartWrapper* chart = Exchanger().chart(id());
    if (chart == nullptr)
        return EMACross();

    return EMACross(chart->get().cbegin(), _it, long_length, short_length);
}