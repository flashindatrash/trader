//
// Created by Вадим Проскурин on 03.10.2021.
//

#include "EMA.hpp"

#include "exchanger/wrapper/CandlestickWrapper.hpp"

using namespace indicator;

EMA::EMA(size_t length)
    : _length(length)
{
}

bool EMA::load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) {
    if (std::distance(begin, end) < (long)_length + 1)
        return false;

    for (; begin <= end; ++begin)
        add((*begin)->priceClose());

    return true;
}

void EMA::add(Price value) {
    /*
     * EMA = Price(t) × k + EMA(y) × (1−k)
     * where:
     * t=today
     * y=yesterday
     * N=number of days in EMA
     * k=2÷(N+1)
    */

    if (_data.empty())
        _data.push_back(value);
    else
        _data.push_back(value * multiplier() + _data.back() * (1 - multiplier()));
}

double EMA::multiplier() const {
    return 2.0 / ((double)_length + 1.0);
}

const Price& EMA::at(size_t index) const {
    return _data.at(index);
}

bool EMA::empty() const {
    return _data.empty();
}

size_t EMA::size() const {
    return _data.size();
}

Price EMA::last() const {
    return _data.back();
}

Price EMA::prev() const {
    return _data.at(size() - 2);
}