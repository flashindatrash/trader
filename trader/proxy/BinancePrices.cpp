#include "BinancePrices.hpp"

#include "binacpp.h"
#include "binacpp_websocket.h"
#include "Logger.hpp"
#include "proxy/BinancePrices.hpp"
#include "wrapper/BinanceSymbol.hpp"
#include "wrapper/BinancePriceHistory.hpp"
#include "data/BinanceBookData.hpp"

BinancePrices::~BinancePrices() {
    for (auto& pair : _histories)
        delete pair.second;
    _histories.clear();
}

void BinancePrices::init() {
    Json::Value result;
    BinaCPP::get_allPrices( result );

    if (not result.isArray()) {
        trace("%s\n", result.toStyledString().c_str());
        return;
    }

    for (uint i = 0; i < result.size(); ++i) {
        const Json::Value& data = result[i];

        std::string symbol = data["symbol"].asString();
        double price = atof(data["price"].asString().c_str());

        setPrice(symbol, price);
    }
}

void BinancePrices::connect(const BinanceSymbol& symbol) {
    std::string low_symbol = symbol;
    for (size_t i = 0; i < low_symbol.size(); ++i)
        low_symbol[i] = tolower(low_symbol[i]);

    const std::string& path = "/ws/" + low_symbol + "@bookTicker";
    BinaCPP_websocket::connect_endpoint(std::bind(&BinancePrices::handle, this, std::placeholders::_1), path.c_str());
}

int BinancePrices::handle(Json::Value& json) {
    BinanceBookData result(json);
    BinanceSymbol symbol(result.symbol);

    // update average price
    double avgPrice = (result.bestAskPrice + result.bestBidPrice) / 2.0;
    setPrice(symbol, avgPrice);

    // update history
    BinancePriceHistory* history = nullptr;
    auto it = _histories.find(symbol);
    if (it == _histories.end()) {
        history = BinancePriceHistory::create();
        _histories[symbol] = history;
    } else
        history = it->second;
    history->add(avgPrice);

    // invoke listeners
    for (Fn& listener : _listeners)
        listener(result);

    return 0;
}

void BinancePrices::addListener(Fn listener) {
    _listeners.push_back(listener);
}

void BinancePrices::setPrice(const BinanceSymbol& symbol, double price) {
    _prices[symbol] = price;
}

double BinancePrices::getPrice(const BinanceSymbol& symbol) const {
    auto it = _prices.find(symbol);
    if (it == _prices.end())
        return 0.0;
    return it->second;
}

const BinancePriceHistory* BinancePrices::getPriceHistory(const BinanceSymbol& symbol) const {
    auto it = _histories.find(symbol);
    if (it == _histories.end())
        return nullptr;
    return it->second;
}
