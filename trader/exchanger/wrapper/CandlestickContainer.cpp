#include "CandlestickContainer.hpp"
#include "Logger.hpp"

CandlestickContainer* CandlestickContainer::create() {
    CandlestickContainer* wrapper = new CandlestickContainer();
    return wrapper;
}

void CandlestickContainer::add(const BinanceKlineData& data) {
    if (_klines.empty() || data.timeStart > _klines.back().timeStart) {
        // если предыдущая не закрылась
        if (not _klines.empty() && not _klines.back().isClosed) {
            _klines.back().isClosed = true;
            invoke(_klines.back());
        }
        _klines.push_back(data);
    } else if (data.timeStart == _klines.back().timeStart) {
        _klines.back() = data;
        if (_klines.back().isClosed)
            invoke(_klines.back());
    } else
        Logger::error("kline back in time");
}

const std::vector<BinanceKlineData>& CandlestickContainer::klines() const {
    return _klines;
}

const BinanceKlineData& CandlestickContainer::back() const {
    return _klines.back();
}
