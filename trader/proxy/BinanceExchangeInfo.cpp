#include "BinanceExchangeInfo.hpp"

#include "binacpp.h"

void BinanceExchangeInfo::init() {
    Json::Value result;
    BinaCPP::get_exchangeInfo(result);

    const Json::Value& symbols = result["symbols"];
    if (not symbols.isArray())
        return;

    for (uint i = 0; i < symbols.size(); ++i) {
        BinanceSymbolData data(symbols[i]);
        _symbols[data.symbol] = data;
    }
}

const BinanceSymbolData& BinanceExchangeInfo::getSymbolInfo(const std::string& symbol) const {
    return _symbols.at(symbol);
}
