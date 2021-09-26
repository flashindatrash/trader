#include "CandlestickWrapper.hpp"

CandlestickWrapper* CandlestickWrapper::create() {
    auto* wrapper = new CandlestickWrapper();
    return wrapper;
}

void CandlestickWrapper::set(const Candlestick& data) {
    _identifier = data.symbol;
    _data = data;
}

const Price& CandlestickWrapper::priceOpen() const {
    return _data.price_open;
}

const Price& CandlestickWrapper::priceClose() const {
    return _data.price_close;
}

Price CandlestickWrapper::priceMin() const {
    return std::min(_data.price_open, _data.price_close);
}

Price CandlestickWrapper::priceMax() const {
    return std::max(_data.price_open, _data.price_close);
}

Price CandlestickWrapper::bodyLen() const {
    return std::abs(_data.price_open - _data.price_close);
}

Price CandlestickWrapper::wickLen() const {
    return _data.price_high - priceMax();
}

Price CandlestickWrapper::tailLen() const {
    return priceMin() - _data.price_low;
}

bool CandlestickWrapper::isBullish() const {
    return _data.price_open < _data.price_close;
}

bool CandlestickWrapper::isBearish() const {
    return _data.price_open > _data.price_close;
}

Price CandlestickWrapper::hl2() const {
    return (_data.price_high + _data.price_low) / 2.0;
}

Price CandlestickWrapper::hlc3() const {
    return (_data.price_high + _data.price_low + _data.price_close) / 3.0;
}

Price CandlestickWrapper::ohlc4() const {
    return (_data.price_open + _data.price_high + _data.price_low + _data.price_close) / 4.0;
}

const time_t& CandlestickWrapper::timeOpen() const {
    return _data.time_open;
}

const time_t& CandlestickWrapper::timeClose() const {
    return _data.time_close;
}

const bool& CandlestickWrapper::isClosed() const {
    return _data.closed;
}

void CandlestickWrapper::close() {
    _data.closed = true;
}

