//
// Created by Вадим Проскурин on 03.10.2021.
//

#include "EMACross.hpp"

EMACross::EMACross(size_t fast, size_t slow)
    : _fast(fast)
    , _slow(slow)
{
}

bool EMACross::load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) {
    return _fast.load(begin, end) && _slow.load(begin, end);
}

OrderSide EMACross::trend() const {
    if (empty())
        return OrderSide::Invalid;

    return compare(_fast.last(), _slow.last());
}

OrderSide EMACross::compare(Price fast, Price slow) {
    return fast > slow ? OrderSide::Buy : OrderSide::Sell;
}

bool EMACross::crossed() const {
    if (_fast.size() < 2 || _slow.size() < 2)
        return false;

    return trend() != compare(_fast.prev(), _slow.prev());
}

bool EMACross::empty() const {
    return _fast.empty() || _slow.empty();
}
