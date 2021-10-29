//
// Created by Вадим Проскурин on 03.10.2021.
//

#include "DEMA.hpp"

DEMA::DEMA(size_t fast, size_t slow)
    : _fast(fast)
    , _slow(slow)
{
}

bool DEMA::load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) {
    return _fast.load(begin, end) && _slow.load(begin, end);
}

OrderSide DEMA::trend() const {
    if (empty())
        return Invalid;

    return compare(_fast.last(), _slow.last());
}

OrderSide DEMA::signal() const {
    return crossed() ? trend() : Invalid;
}

OrderSide DEMA::compare(Price fast, Price slow) {
    return fast > slow ? Buy : Sell;
}

bool DEMA::crossed() const {
    if (_fast.size() < 2 || _slow.size() < 2)
        return false;

    return trend() != compare(_fast.prev(), _slow.prev());
}

bool DEMA::empty() const {
    return _fast.empty() || _slow.empty();
}
