#include "BinancePrices.hpp"

#include "binacpp.h"
#include "Logger.hpp"

void BinancePrices::init() {
    Json::Value result;
    BinaCPP::get_allPrices( result );

    if (not result.isArray()) {
        log("%s\n", result.toStyledString().c_str());
        return;
    }

    for (uint i = 0; i < result.size(); ++i) {
        const Json::Value& data = result[i];

        std::string symbol = data["symbol"].asString();
        double price = atof(data["price"].asString().c_str());

        setPrice(symbol, price);
    }
}

void BinancePrices::setPrice(const std::string& symbol, double price) {
    _prices[symbol] = price;
}

double BinancePrices::getPrice(const std::string &symbol) const {
    if (_prices.find(symbol) == _prices.end())
        return 0.0;

    return _prices.at(symbol);
}
