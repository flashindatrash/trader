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

const CandlestickWrapper* ChartWrapper::last() const {
    if (_candlesticks.empty())
        return nullptr;

    return _candlesticks.back();
}

ChartWrapper::Range ChartWrapper::last(Price current, Price change) const {
    Range range;

    if (current == change || _candlesticks.size() < 3)
        return range;

    for (auto it = _candlesticks.crbegin() + 1; it < _candlesticks.crend(); ++it) {
        const CandlestickWrapper* candlestick = *it;

        // find end
        if (range.end == nullptr) {
            if ((change > current && candlestick->priceMin() > change) || (change < current && candlestick->priceMax() < change))
                range.end = candlestick;
            continue;
        }

        // find begin
        if (range.begin == nullptr) {
            if ((change > current && candlestick->priceMax() < current) || (change < current && candlestick->priceMin() > current))
                range.begin = candlestick;
            continue;
        }
    }
    return range;
}

void ChartWrapper::setInterval(ChartInterval interval) {
    _interval = interval;
}

const ChartInterval& ChartWrapper::interval() const {
    return _interval;
}

bool ChartWrapper::Range::isValid() const {
    return begin != nullptr && end != nullptr;
}