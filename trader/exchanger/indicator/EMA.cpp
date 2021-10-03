//
// Created by Вадим Проскурин on 03.10.2021.
//

#include "EMA.hpp"

#include "exchanger/wrapper/CandlestickWrapper.hpp"

EMA::EMA(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end, size_t length) {
    if (std::distance(begin, end) < (long)length + 1)
        return;

    _data.push_back((*(begin++))->priceClose());

    /*
     * EMA = Price(t) × k + EMA(y) × (1−k)
     * where:
     * t=today
     * y=yesterday
     * N=number of days in EMA
     * k=2÷(N+1)
    */
    double k = 2.0 / ((double)length + 1.0);

    for (; begin <= end; ++begin)
        _data.push_back((*begin)->priceClose() * k + _data.back() * (1 - k));
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