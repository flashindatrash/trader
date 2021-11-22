#include "Context.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/indicator/BaseIndicator.hpp"

NS_USE

const Context* Context::current = nullptr;

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

bool Context::load(BaseIndicator& indicator) const {
    const ChartWrapper* chart = Exchanger().chart(id());
    if (chart == nullptr)
        return false;

    return indicator.load(chart->get().cbegin(), _it);
}