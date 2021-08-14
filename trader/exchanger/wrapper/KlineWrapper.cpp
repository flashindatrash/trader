#include "KlineWrapper.hpp"

KlineWrapper* KlineWrapper::create() {
    KlineWrapper* wrapper = new KlineWrapper();
    return wrapper;
}

void KlineWrapper::setPrice(Price open, Price high, Price low, Price close) {
    _open = open;
    _high = high;
    _low = low;
    _close = close;
}

const Price& KlineWrapper::open() const {
    return _open;
}

const Price& KlineWrapper::close() const {
    return _close;
}

void KlineWrapper::setTime(time_t open, time_t close) {
    _time_open = open;
    _time_close = close;
}

void KlineWrapper::setTimeClose(time_t close) {
    _time_close = close;
}

const time_t& KlineWrapper::timeClose() const {
    return _time_close;
}
