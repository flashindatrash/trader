#include "binacpp.h"
#include "binacpp_websocket.h"
#include "Logger.hpp"
#include "proxy/BinancePrices.hpp"
#include "wrapper/TradeSymbol.hpp"
#include "wrapper/PriceHistory.hpp"
#include "data/BinanceBookData.hpp"
#include "util/StringUtil.hpp"

BinancePrices::~BinancePrices() {
    for (auto& pair : _histories)
        SAFE_DELETE(pair.second);
    _histories.clear();
}

void BinancePrices::init() {
    Json::Value result;
    BinaCPP::get_allPrices(result);

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

void BinancePrices::connect(const TradeSymbol& symbol) {
    const std::string& path = "/ws/" + util::lowercase(symbol.c_str()) + "@bookTicker";
    BinaCPP_websocket::connect_endpoint(std::bind(&BinancePrices::handle, this, std::placeholders::_1), path.c_str());
}

int BinancePrices::handle(Json::Value& json) {
    BinanceBookData result(json);
    TradeSymbol symbol(result.symbol);

    // update average price
    double avgPrice = (result.bestAskPrice + result.bestBidPrice) / 2.0;
    setPrice(symbol, avgPrice);

    // update history
    PriceHistory* history = nullptr;
    auto it = _histories.find(symbol);
    if (it == _histories.end()) {
        history = PriceHistory::create();
        _histories[symbol] = history;
    } else
        history = it->second;
    history->add(avgPrice);

    // invoke listeners
    invoke(result);
    return 0;
}

void BinancePrices::setPrice(const TradeSymbol& symbol, double price) {
    _prices[symbol] = price;
}

double BinancePrices::getPrice(const TradeSymbol& symbol) const {
    auto it = _prices.find(symbol);
    if (it == _prices.end())
        return 0.0;
    return it->second;
}

const PriceHistory* BinancePrices::getHistory(const TradeSymbol& symbol) const {
    auto it = _histories.find(symbol);
    if (it == _histories.end())
        return nullptr;
    return it->second;
}
