#include "ChartWrapper.hpp"
#include "CandlestickWrapper.hpp"

ChartWrapper* ChartWrapper::create() {
    ChartWrapper* wrapper = new ChartWrapper();
    return wrapper;
}

Signal<const CandlestickWrapper&> ChartWrapper::onCandleClosed;

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
