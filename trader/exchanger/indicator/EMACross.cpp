//
// Created by Вадим Проскурин on 03.10.2021.
//

#include "EMACross.hpp"

EMACross::EMACross(size_t long_length, size_t short_length)
    : _long(long_length)
    , _short(short_length)
{
}

bool EMACross::load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) {
    return _long.load(begin, end) && _short.load(begin, end);
}

OrderSide EMACross::trend() const {
    if (empty())
        return OrderSide::Invalid;

    return compare(_long.last(), _short.last());
}

OrderSide EMACross::compare(Price ema_long, Price ema_short) {
    return ema_short > ema_long ? OrderSide::Buy : OrderSide::Sell;
}

bool EMACross::crossed() const {
    if (_long.size() < 2 || _short.size() < 2)
        return false;

    return trend() != compare(_long.prev(), _short.prev());
}

bool EMACross::empty() const {
    return _long.empty() || _short.empty();
}
