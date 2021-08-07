#include "wrapper/KlineHistory.hpp"
#include "Logger.hpp"

KlineHistory* KlineHistory::create() {
    KlineHistory* wrapper = new KlineHistory();
    return wrapper;
}

void KlineHistory::add(const BinanceKlineData &data) {
    if (_klines.empty() || data.timeStart > _klines.back().timeStart) {
        if (not _klines.empty())
            _klines.back().isClosed = true;
        _klines.push_back(data);
    } else if (data.timeStart == _klines.back().timeStart) {
        _klines.back() = data;
    } else
        logic_error("kline back in time");
}

const BinanceKlineData& KlineHistory::back() const {
    return _klines.back();
}
