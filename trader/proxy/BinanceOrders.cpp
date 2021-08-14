#include "binacpp.h"
#include "Logger.hpp"
#include "Config.hpp"
#include "proxy/BinanceOrders.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/binance/response/BinanceSymbolData.hpp"
#include "exchanger/binance/response/BinanceErrorData.hpp"

std::vector<BinanceOrderData> BinanceOrders::getAllOrders(const Symbol& symbol, int limit/* = 0*/) const {
    Json::Value result;
    BinaCPP::get_allOrders(symbol.c_str(), 0, limit, BINANCE_RECV_WINDOW, result);

    std::vector<BinanceOrderData> vec;

    BinanceErrorData error(result);
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return vec;
    }

    if (not result.isArray()) {
        trace("%s\n", result.toStyledString().c_str());
        Logger::error("invalid orders");
        return vec;
    }

    for (uint i = 0; i < result.size(); ++i)
        vec.push_back(BinanceOrderData(result[i], false));

    return vec;
}

bool BinanceOrders::isEnough(const Symbol& symbol, const SideEnum& side, double quantity) const {
    if (side == SideEnum::Buy)
        return symbol.quoteAsset().getBalance() >= symbol.getPrice(quantity);
    else
        return symbol.baseAsset().getBalance() > quantity;
}

const BinanceOrderData BinanceOrders::createOrder(const Symbol& symbol, const SideEnum& side, double quantity) const {
    const BinanceSymbolData& info = symbol.getInfo();

    // check minNotional
    if (info.minNotional.applyToMarket) {
        double avgPrice = symbol.getPrice();
        if (const PriceWrapper* history = Exchanger().price(symbol))
            avgPrice = history->getPriceAverage(info.minNotional.avgPriceMins * BinanceTime::sMinute);

        if (avgPrice * quantity < info.minNotional.minNotional) {
            trace("can't trade %f %s less minNotional\n", quantity, symbol.c_str());
            return BinanceOrderData();
        }
    }

    return createOrder(symbol, side, quantity, "MARKET");
}

const BinanceOrderData BinanceOrders::createOrder(const Symbol& symbol, const SideEnum& side, double quantity, const std::string& type) const {
    const BinanceSymbolData& info = symbol.getInfo();

    // check if symbol can trade on market
    if (not info.hasOrderType(type)) {
        trace("can't trade symbol %s on %s\n", symbol.c_str(), type.c_str());
        return BinanceOrderData();
    }

    // check lot size
    if (info.lotSize.has) {
        if (quantity < info.lotSize.minQty) {
            trace("can't trade %f quantity (min %f)\n", quantity, info.lotSize.minQty);
            return BinanceOrderData();
        } else if (quantity > info.lotSize.maxQty) {
            trace("can't trade %f quantity (max %f)\n", quantity, info.lotSize.maxQty);
            return BinanceOrderData();
        }
        /* todo
        else if ((quantity - info.lotSize.minQty) % info.lotSize.stepSize != 0) {
            trace("can't trade %f quantity (step size %f)\n", quantity, info.lotSize.stepSize);
            return BinanceOrderData();
        }*/
    }

    Json::Value result;
    BinaCPP::send_order(symbol.c_str(), side.c_str(), type.c_str(), "GTC", quantity , 0, "", 0, 0, BINANCE_TEST_MODE, BINANCE_RECV_WINDOW, result);

    // error response
    BinanceErrorData error(result);
    if (error.has()) {
        BinanceOrderData empty;
        if (error.code == BinanceErrorData::NEW_ORDER_REJECTED) {
            empty.status = "REJECTED";
            const Asset& asset = side == SideEnum::Buy ? symbol.quoteAsset() : symbol.baseAsset();
            double required = side == SideEnum::Buy ? symbol.getPrice(quantity) : quantity;
            trace("don't have %f %s (current %f)\n", required, asset.c_str(), asset.getBalance());
        } else {
            trace("%s\n", error.msg.c_str());
        }

        return empty;
    }

    return BinanceOrderData(result, false);
}
