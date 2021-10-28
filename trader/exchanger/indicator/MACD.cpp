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
    return true;
}