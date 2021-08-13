#include "binacpp.h"
#include "Logger.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/BinancePrices.hpp"
#include "exchanger/wrapper/TradeSymbol.hpp"
#include "exchanger/wrapper/PriceSymbol.hpp"
#include "exchanger/binance/response/BinanceErrorData.hpp"
#include "exchanger/binance/response/BinancePriceStatisticsData.hpp"

BinancePrices::~BinancePrices() {
    for (auto& pair : _symbols)
        SAFE_DELETE(pair.second);
    _symbols.clear();
}

void BinancePrices::init() {
    Json::Value result;
    BinaCPP::get_allPrices(result);

    BinanceErrorData error(result);
    if (error.has()) {
        logic_error(error.msg.c_str());
        return;
    }

    if (not result.isArray()) {
        trace("%s\n", result.toStyledString().c_str());
        logic_error("invalid prices");
        return;
    }

    for (uint i = 0; i < result.size(); ++i) {
        const Json::Value& data = result[i];

        std::string symbol = data["symbol"].asString();
        double price = atof(data["price"].asString().c_str());

        PriceSymbol* wrapper = PriceSymbol::create();
        wrapper->add(price);

        _symbols[symbol] = wrapper;
    }
}

const BinancePriceStatisticsData& BinancePrices::getStats(const TradeSymbol& symbol) {
    static const BinancePriceStatisticsData sEmpty;

    PriceSymbol* wrapper = getMutablePrice(symbol);
    if (wrapper == nullptr)
        return sEmpty;

    BinancePriceStatisticsData& stats = wrapper->getStats();

    // lazy update from api
    time_t now = STime().getCurrent();
    if (now - BinanceTime::sMinute * 10 > stats.closeTime) {
        Json::Value result;
        BinaCPP::get_24hr(symbol.c_str(), result);

        BinanceErrorData error(result);
        if (error.has()) {
            logic_error(error.msg.c_str());
            return stats;
        }

        stats = BinancePriceStatisticsData(result);
    }

    return stats;
}

const PriceSymbol* BinancePrices::getPrice(const TradeSymbol& symbol) const {
    auto it = _symbols.find(symbol);
    if (it == _symbols.end())
        return nullptr;
    return it->second;
}

PriceSymbol* BinancePrices::getMutablePrice(const TradeSymbol& symbol) const {
    auto it = _symbols.find(symbol);
    if (it == _symbols.end())
        return nullptr;
    return it->second;
}
