#include "SymbolInfo.hpp"

SymbolInfo* SymbolInfo::create()
{
    SymbolInfo* wrapper = new SymbolInfo();
    return wrapper;
}

void SymbolInfo::setAssets(const Asset& base, const Asset& quote) {
    _baseAsset = base;
    _quoteAsset = quote;
}

const Asset& SymbolInfo::baseAsset() const {
    return _baseAsset;
}

const Asset& SymbolInfo::quoteAsset() const {
    return _quoteAsset;
}
