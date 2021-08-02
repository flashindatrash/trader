#include "BinanceBook.hpp"

#include "binacpp_websocket.h"
#include "BinanceSymbol.hpp"
#include "proxy/BinancePrices.hpp"
#include "data/BinanceBookData.hpp"

void BinanceBook::connect(const BinanceSymbol& symbol) {
    std::string low_symbol = symbol;
    for (size_t i = 0; i < low_symbol.size(); ++i)
        low_symbol[i] = tolower(low_symbol[i]);

    const std::string& path = "/ws/" + low_symbol + "@bookTicker";
    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceBook::handle, this, std::placeholders::_1), path.c_str());
}

int BinanceBook::handle(Json::Value& json) {
    BinanceBookData result(json);

    double avgPrice = (result.bestAskPrice + result.bestBidPrice) / 2.0;
    SBinancePrices().setPrice(result.symbol, avgPrice);

    for (Fn& listener : _listeners)
        listener(result);

    return 0;
}

void BinanceBook::addListener(Fn listener) {
    _listeners.push_back(listener);
}
