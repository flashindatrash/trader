#include "BinanceController.hpp"
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"
#include "Config.hpp"
#include "Logger.hpp"
#include "exchanger/wrapper/SymbolSet.hpp"
#include "exchanger/wrapper/PriceContainer.hpp"
#include "exchanger/wrapper/SymbolInfo.hpp"
#include "exchanger/wrapper/Balance.hpp"
#include "response/BinanceErrorData.hpp"
#include "response/BinanceSymbolData.hpp"
#include "response/BinanceBalanceData.hpp"

BinanceController::~BinanceController() {
    if (_thread.joinable())
        _thread.join();
}

void BinanceController::init(const core::Config& config) {
    // init binance logger
    BinaCPP_logger::set_debug_level(2);
    BinaCPP_logger::enable_logfile(1);

    // init binance api
    static string api_key       = config.getAsString("BINANCE_API_KEY");
    static string secret_key    = config.getAsString("BINANCE_SECRET_KEY");

    BinaCPP::init(api_key, secret_key);
    BinaCPP_websocket::init();
}

void BinanceController::run() {
    _thread = std::thread(&BinaCPP_websocket::enter_event_loop);
}

bool BinanceController::getSymbolInfo(SymbolSet<SymbolInfo>& result) {
    Json::Value json;
    BinaCPP::get_exchangeInfo(json);

    BinanceErrorData error(json);
    if (error.has()) {
        logic_error(error.msg.c_str());
        return false;
    }

    const Json::Value& symbols = json["symbols"];
    if (not symbols.isArray()) {
        trace("%s\n", json.toStyledString().c_str());
        logic_error("invalid exchange");
        return false;
    }

    for (uint i = 0; i < symbols.size(); ++i) {
        BinanceSymbolData data(symbols[i]);

        SymbolInfo* wrapper = result.get_mutable(data.symbol);
        wrapper->setAssets(data.baseAsset, data.quoteAsset);
    }

    return true;
}

bool BinanceController::getAllPrices(SymbolSet<PriceContainer>& result) {
    Json::Value json;
    BinaCPP::get_allPrices(json);

    BinanceErrorData error(json);
    if (error.has()) {
        logic_error(error.msg.c_str());
        return false;
    }

    if (not json.isArray()) {
        trace("%s\n", json.toStyledString().c_str());
        logic_error("invalid prices");
        return false;
    }

    for (uint i = 0; i < json.size(); ++i) {
        const Json::Value& data = json[i];
        std::string symbol = data["symbol"].asString();
        Price price = atof(data["price"].asString().c_str());
        result.get_mutable(symbol)->add(price);
    }

    return true;
}

bool BinanceController::getBalances(SymbolSet<Balance>& result) {
    Json::Value json;
    BinaCPP::get_account(BINANCE_RECV_WINDOW, json);

    BinanceErrorData error(json);
    if (error.has()) {
        logic_error(error.msg.c_str());
        return false;
    }

    const Json::Value& balances = json["balances"];
    if (not balances.isArray()) {
        trace("%s\n", json.toStyledString().c_str());
        logic_error("invalid account");
        return false;
    }

    for (uint i = 0; i < balances.size(); ++i) {
        BinanceBalanceData data(balances[i], false);
        result.get_mutable(data.asset)->set(data.free, data.locked);
    }

    return true;
}
