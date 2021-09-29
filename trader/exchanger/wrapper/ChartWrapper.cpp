#include "ChartWrapper.hpp"
#include "CandlestickWrapper.hpp"

ChartWrapper* ChartWrapper::create() {
    auto* wrapper = new ChartWrapper();
    return wrapper;
}

Signal<CandlestickWrapper> ChartWrapper::onCandleClosed;

ChartWrapper::~ChartWrapper() {
    for (CandlestickWrapper* candlestick : _candlesticks)
        delete candlestick;
    _candlesticks.clear();
}

const CandlestickWrapper* ChartWrapper::add(const Candlestick& data) {
    CandlestickWrapper* last = _candlesticks.empty() ? nullptr : _candlesticks.back();

    if (last == nullptr || data.time_open > last->timeOpen()) {
        // если предыдущая не закрылась
        if (last && not last->isClosed()) {
            last->close();
            onCandleClosed.emmit(*last);
        }
        CandlestickWrapper* wrapper = CandlestickWrapper::create();
        wrapper->set(data);
        _candlesticks.push_back(wrapper);
        return wrapper;
    }

    if (data.time_open == last->timeOpen()) {
        last->set(data);
        if (data.closed)
            onCandleClosed.emmit(*last);
        return last;
    }

    return nullptr;
}

const std::vector<CandlestickWrapper*>& ChartWrapper::get() const {
    return _candlesticks;
}

Price ChartWrapper::ema(ConstIterator end, size_t length) const {
    if (std::distance(_candlesticks.cbegin(), end) < length + 1)
        return 0.0;

    auto it = end - (int)length - 1;
    Price result = (*(it++))->priceClose();

    /*
     * EMA = Price(t) × k + EMA(y) × (1−k)
     * where:
     * t=today
     * y=yesterday
     * N=number of days in EMA
     * k=2÷(N+1)
    */
    double k = 2.0 / (length + 1.0);

    for (; it < end; ++it)
        result = (*it)->priceClose() * k + result * (1 - k);

    return result;
}
