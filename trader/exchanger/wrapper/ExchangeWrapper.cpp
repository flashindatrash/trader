#include "ExchangeWrapper.hpp"

ExchangeWrapper* ExchangeWrapper::create()
{
    ExchangeWrapper* wrapper = new ExchangeWrapper();
    return wrapper;
}

void ExchangeWrapper::setAssets(const Asset& base, const Asset& quote) {
    _baseAsset = base;
    _quoteAsset = quote;
}

const Asset& ExchangeWrapper::baseAsset() const {
    return _baseAsset;
}

const Asset& ExchangeWrapper::quoteAsset() const {
    return _quoteAsset;
}
