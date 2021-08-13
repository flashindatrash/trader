#include "KlineHistory.hpp"
#include "Logger.hpp"

KlineHistory* KlineHistory::create() {
    KlineHistory* wrapper = new KlineHistory();
    return wrapper;
}

void KlineHistory::add(const BinanceKlineData& data) {
    if (_klines.empty() || data.timeStart > _klines.back().timeStart) {
        if (not _klines.empty()) {
            _klines.back().isClosed = true;
            invoke(_klines.back());
        }
        _klines.push_back(data);
    } else if (data.timeStart == _klines.back().timeStart) {
        _klines.back() = data;
        if (_klines.back().isClosed)
            invoke(_klines.back());
    } else
        logic_error("kline back in time");
}

const std::vector<BinanceKlineData>& KlineHistory::klines() const {
    return _klines;
}

const BinanceKlineData& KlineHistory::back() const {
    return _klines.back();
}
