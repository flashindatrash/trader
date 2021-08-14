#include "BinanceController.hpp"
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"
#include "Config.hpp"
#include "Logger.hpp"
#include "proxy/BinanceTime.hpp"
#include "exchanger/wrapper/SymbolSet.hpp"
#include "exchanger/wrapper/PriceContainer.hpp"
#include "exchanger/wrapper/SymbolInfo.hpp"
#include "exchanger/wrapper/Balance.hpp"
#include "response/BinanceErrorData.hpp"
#include "response/BinanceSymbolData.hpp"
#include "response/BinanceBalanceData.hpp"
#include "response/BinanceOrderData.hpp"

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

    initUserListenKey();
}

void BinanceController::run() {
    _thread = std::thread(&BinaCPP_websocket::enter_event_loop);
}

void BinanceController::tick(time_t now) {
    // Keepalive a user data stream to prevent a time out.
    // User data streams will close after 60 minutes.
    // It's recommended to send a ping about every 30 minutes
    if (now >= _stream_keep_alive + BinanceTime::sMinute * 30) {
        BinaCPP::keep_userDataStream(_stream_listen_key.c_str());
        _stream_keep_alive = now;
    }
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

void BinanceController::connectBalances(SymbolSet<Balance>& result, Signal<Asset>& signal) {
    _balance_container = &result;
    startUserDataStream();
}

void BinanceController::initUserListenKey() {
    Json::Value json;

    BinaCPP::start_userDataStream(json);
    BinanceErrorData error(json);
    if (error.has()) {
        logic_error(error.msg.c_str());
        return;
    }

    if (!json["listenKey"] || !json["listenKey"].isString()) {
        trace("%s\n", json.toStyledString().c_str());
        logic_error("can't get listenKey for stream account");
        return;
    }

    _stream_listen_key = json["listenKey"].asString();
    _stream_keep_alive = STime().getCurrent();
}

void BinanceController::startUserDataStream() {
    std::string ws_path = std::string("/ws/");
    ws_path.append(_stream_listen_key);

    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceController::onUserDataStream, this, std::placeholders::_1), ws_path.c_str());
}

int BinanceController::onUserDataStream(Json::Value &json) {
    std::string action = json["e"].asString();
    if (action  == "executionReport") {
        std::string executionType = json["x"].asString();
        if (executionType == "NEW") {
            BinanceOrderData order(json, true);
            if (order.isRejected())
                logic_error(json["r"].asString().c_str());
        }
    } else if (action == "outboundAccountPosition") {
        for (uint i = 0; i < json["B"].size(); ++i) {
            BinanceBalanceData data(json["B"][i], true);
            if (_balance_container != nullptr)
                _balance_container->get_mutable(data.asset)->set(data.free, data.locked);
        }
    }

    return 0;
}
