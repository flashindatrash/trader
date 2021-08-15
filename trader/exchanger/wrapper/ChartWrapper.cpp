#include "ChartWrapper.hpp"
#include "Logger.hpp"
#include "CandlestickWrapper.hpp"

ChartWrapper* ChartWrapper::create() {
    ChartWrapper* wrapper = new ChartWrapper();
    return wrapper;
}

bool ChartWrapper::add(const Candlestick& data) {
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
        return true;
    }

    if (data.time_open == last->timeOpen()) {
        last->set(data);
        if (data.closed)
            onCandleClosed.emmit(*last);
        return true;
    }

    return false;
}

const std::vector<CandlestickWrapper*>& ChartWrapper::klines() const {
    return _candlesticks;
}
