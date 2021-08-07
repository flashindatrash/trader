#include "binacpp.h"
#include "Logger.hpp"
#include "proxy/BinanceExchangeInfo.hpp"
#include "data/BinanceErrorData.hpp"

void BinanceExchangeInfo::init() {
    Json::Value result;
    BinaCPP::get_exchangeInfo(result);

    BinanceErrorData error(result);
    if (error.has()) {
        runtime_error(error.msg.c_str());
        return;
    }

    const Json::Value& symbols = result["symbols"];
    if (not symbols.isArray()) {
        trace("%s\n", result.toStyledString().c_str());
        runtime_error("invalid exchange");
        return;
    }

    for (uint i = 0; i < symbols.size(); ++i) {
        BinanceSymbolData data(symbols[i]);
        _symbols[data.symbol] = data;
    }
}

const BinanceSymbolData& BinanceExchangeInfo::getSymbolInfo(const std::string& symbol) const {
    return _symbols.at(symbol);
}
