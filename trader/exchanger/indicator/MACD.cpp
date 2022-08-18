//
// Created by Вадим Проскурин on 29.10.2021.
//

#include "MACD.hpp"

MACD::MACD(size_t fast, size_t slow, size_t signal)
    : _fast(fast)
    , _slow(slow)
    , _signal(signal)
{
}

bool MACD::load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) {
    if (not _fast.load(begin, end) || not _slow.load(begin, end))
        return false;

    for (size_t i = 0; i < _fast.size(); ++i)
        _signal.add(_fast.at(i) - _slow.at(i));
    return true;
}

OrderSide MACD::trend() const {
    if (empty())
        return Invalid;

    return _signal.last() > 0LL ? Buy : _signal.last() < 0LL ? Sell : Invalid;
}

OrderSide MACD::signal() const {
    return crossed() ? trend() : Invalid;
}

bool MACD::crossed() const {
    if (_signal.size() < 2)
        return false;

    return (_signal.prev() <= 0LL && _signal.last() >= 0LL) || (_signal.prev() >= 0LL && _signal.last() <= 0LL);
}

bool MACD::empty() const {
    return _signal.empty();
}