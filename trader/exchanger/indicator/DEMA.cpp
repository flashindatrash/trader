//
// Created by Вадим Проскурин on 03.10.2021.
//

#include "DEMA.hpp"

using namespace indicator;

DEMA::DEMA(size_t fast, size_t slow, Decimal threshold)
    : _fast(fast)
    , _slow(slow)
    , _threshold(threshold)
{
}

bool DEMA::load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) {
    return _fast.load(begin, end) && _slow.load(begin, end);
}

OrderSide DEMA::trend() const {
    if (empty())
        return Invalid;

    const Price fast = _fast.last();
    const Price slow = _slow.last();
    if (_threshold == Decimal::Zero)
        return compare(fast, slow);

    const Price threshold = slow * _threshold;
    if (fast - slow > threshold)
        return Buy;
    if (slow - fast > threshold)
        return Sell;

    return Invalid;
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
