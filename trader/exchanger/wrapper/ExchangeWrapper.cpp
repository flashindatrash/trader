#include "ExchangeWrapper.hpp"

ExchangeWrapper* ExchangeWrapper::create() {
    ExchangeWrapper* wrapper = new ExchangeWrapper();
    return wrapper;
}

void ExchangeWrapper::set(SymbolData data) {
    _data = data;
}

const Asset& ExchangeWrapper::baseAsset() const {
    return _data.baseAsset;
}

const Asset& ExchangeWrapper::quoteAsset() const {
    return _data.quoteAsset;
}

bool ExchangeWrapper::hasOrderType(const OrderType& type) const {
    return _data.hasOrderType(type);
}
