#include "PriceWrapper.hpp"
#include "core/Time.hpp"

PriceWrapper* PriceWrapper::create() {
    auto* wrapper = new PriceWrapper();
    return wrapper;
}

void PriceWrapper::set(Price price) {
    _price = price;
}

void PriceWrapper::set(Ticker ticker) {
    _ticker = std::move(ticker);
    _ticker.time = Time().ms();
}

const Price& PriceWrapper::get() const {
    return _price;
}

const Price& PriceWrapper::get(const OrderSide& side) const {
    if (_ticker.time > Time().ms() - Timer::sMinute) {
        switch (side) {
            case OrderSide::Sell: return _ticker.bestBidPrice;
            case OrderSide::Buy: return _ticker.bestAskPrice;
            case OrderSide::Invalid: return get();
        }
    }
    return get();
}

const Ticker& PriceWrapper::ticker() const {
    return _ticker;
}