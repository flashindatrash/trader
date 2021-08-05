#include "binacpp.h"
#include "Logger.hpp"
#include "Config.hpp"
#include "proxy/BinanceOrders.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/BinancePrices.hpp"
#include "wrapper/BinanceSymbol.hpp"
#include "wrapper/BinancePriceHistory.hpp"
#include "data/BinanceSymbolData.hpp"
#include "data/BinanceErrorData.hpp"

std::vector<BinanceOrderData> BinanceOrders::getAllOrders(const BinanceSymbol& symbol, int limit/* = 0*/) const {
    Json::Value result;
    BinaCPP::get_allOrders(symbol.c_str(), 0, limit, BINANCE_RECV_WINDOW, result);

    std::vector<BinanceOrderData> vec;
    if (!result.isArray())
        return vec;

    for (uint i = 0; i < result.size(); ++i)
        vec.push_back(BinanceOrderData(result[i], false));

    return vec;
}

const BinanceOrderData BinanceOrders::createOrder(const BinanceSymbol& symbol, const std::string& side, double quantity) const {
    const BinanceSymbolData& info = symbol.getInfo();

    // check minNotional
    if (info.minNotional.applyToMarket) {
        double avgPrice = symbol.getPrice();
        if (const BinancePriceHistory* history = SBinancePrices().getPriceHistory(symbol))
            avgPrice = history->getPriceAverage(info.minNotional.avgPriceMins * BinanceTime::sMinute);

        if (avgPrice * quantity < info.minNotional.minNotional) {
            trace("can't trade %f %s less minNotional\n", quantity, symbol.c_str());
            return BinanceOrderData();
        }
    }

    return createOrder(symbol, side, quantity, "MARKET");
}

const BinanceOrderData BinanceOrders::createOrder(const BinanceSymbol& symbol, const std::string& side, double quantity, const std::string& type) const {
    const BinanceSymbolData& info = symbol.getInfo();

    // check if symbol can trade on market
    if (not info.hasOrderType(type)) {
        trace("can't trade symbol %s on %s\n", symbol.c_str(), type.c_str());
        return BinanceOrderData();
    }

    Json::Value result;
    BinaCPP::send_order(symbol.c_str(), side.c_str(), type.c_str(), "GTC", quantity , 0, "", 0, 0, BINANCE_TEST_MODE, BINANCE_RECV_WINDOW, result);

    // error response
    BinanceErrorData error(result);
    if (error.has()) {
        if (error.code == BinanceErrorData::ERROR_CODE_INSUFFICIENT_BALANCE) {
            const BinanceAsset& asset = side == "BUY" ? symbol.quoteAsset() : symbol.baseAsset();
            double required = side == "BUY" ? symbol.getPrice(quantity) : quantity;
            trace("don't have %f %s (current %f)\n", required, asset.c_str(), asset.getBalance());
        } else
            trace("%s\n", error.msg.c_str());

        return BinanceOrderData();
    }

    return BinanceOrderData(result, false);
}
