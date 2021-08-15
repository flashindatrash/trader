#include "BinanceController.hpp"
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"
#include "Config.hpp"
#include "Logger.hpp"
#include "util/StringUtil.hpp"
#include "proxy/BinanceTime.hpp"
#include "response/BinanceErrorData.hpp"
#include "response/BinanceSymbolData.hpp"
#include "response/BinanceBalanceData.hpp"
#include "response/BinanceOrderData.hpp"
#include "response/BinancePriceStatisticsData.hpp"
#include "response/BinanceKlineData.hpp"

std::string convertChartInterval(ChartInterval interval) {
    switch (interval) {
        case ChartInterval::m5: return "5m";
        case ChartInterval::m15: return "15m";
        default: return "";
    }
}

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
    if (now >= _time_userstream + BinanceTime::sMinute * 30)
        keepUserDataStream();

    if (now >= _time_daily_change + BinanceTime::sMinute * 15)
        updateDailyChange();
}

bool BinanceController::getSymbolInfo(Storage::Type_info& container) const {
    Json::Value json;
    BinaCPP::get_exchangeInfo(json);

    BinanceErrorData error(json);
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    const Json::Value& symbols = json["symbols"];
    if (not symbols.isArray()) {
        trace("%s\n", json.toStyledString().c_str());
        Logger::error("invalid exchange");
        return false;
    }

    for (uint i = 0; i < symbols.size(); ++i) {
        BinanceSymbolData data(symbols[i]);

        ExchangeWrapper* wrapper = container.get(data.symbol);
        wrapper->setAssets(data.baseAsset, data.quoteAsset);
    }

    return true;
}

bool BinanceController::getAllPrices(Storage::Type_price& container) const {
    Json::Value json;
    BinaCPP::get_allPrices(json);

    BinanceErrorData error(json);
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    if (not json.isArray()) {
        trace("%s\n", json.toStyledString().c_str());
        Logger::error("invalid prices");
        return false;
    }

    for (uint i = 0; i < json.size(); ++i) {
        const Json::Value& data = json[i];
        std::string symbol = data["symbol"].asString();
        Price price = atof(data["price"].asString().c_str());
        container.get(symbol)->add(price);
    }

    return true;
}

void BinanceController::connectPrices(Storage::Type_price& container) {
    if (not getAllPrices(container))
        return;

    _connect_prices = &container;
}

bool BinanceController::getBalances(Storage::Type_balance& container) const {
    Json::Value json;
    BinaCPP::get_account(BINANCE_RECV_WINDOW, json);

    BinanceErrorData error(json);
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    const Json::Value& balances = json["balances"];
    if (not balances.isArray()) {
        trace("%s\n", json.toStyledString().c_str());
        Logger::error("invalid account");
        return false;
    }

    for (uint i = 0; i < balances.size(); ++i) {
        BinanceBalanceData data(balances[i], false);
        container.get(data.asset)->set(data.free, data.locked);
    }
    return true;
}

void BinanceController::connectBalances(Storage::Type_balance& container) {
    if (not getBalances(container))
        return;

    _connect_balances = &container;
    startUserDataStream();
}

void BinanceController::initUserListenKey() {
    Json::Value json;

    BinaCPP::start_userDataStream(json);
    BinanceErrorData error(json);
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return;
    }

    if (!json["listenKey"] || !json["listenKey"].isString()) {
        trace("%s\n", json.toStyledString().c_str());
        Logger::error("can't get listenKey for stream account");
        return;
    }

    _stream_listen_key = json["listenKey"].asString();
    _time_userstream = STime().getCurrent();
}

void BinanceController::startUserDataStream() {
    std::string ws_path = std::string("/ws/");
    ws_path.append(_stream_listen_key);

    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceController::onUserDataStream, this, std::placeholders::_1), ws_path.c_str());
}

void BinanceController::keepUserDataStream() {
    if (_stream_listen_key.empty())
        return;

    _time_userstream = STime().getCurrent();
    BinaCPP::keep_userDataStream(_stream_listen_key.c_str());
}

int BinanceController::onUserDataStream(Json::Value &json) {
    BinanceErrorData error(json);
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return 0;
    }

    std::string action = json["e"].asString();
    if (action  == "executionReport") {
        std::string executionType = json["x"].asString();
        if (executionType == "NEW") {
            BinanceOrderData order(json, true);
            if (order.isRejected())
                Logger::error(json["r"].asString().c_str());
        }
    } else if (action == "outboundAccountPosition") {
        for (uint i = 0; i < json["B"].size(); ++i) {
            BinanceBalanceData data(json["B"][i], true);
            if (_connect_balances != nullptr) {
                _connect_balances->get(data.asset)->set(data.free, data.locked);
                _connect_balances->onChanged.emmit(data.asset);
            }
        }
    }

    return 0;
}

void BinanceController::connectDailyChange(CandlestickWrapper& wrapper) {
    _connect_daily_change = &wrapper;
    updateDailyChange();
}

void BinanceController::updateDailyChange() {
    if (_connect_daily_change == nullptr)
        return;

    _time_daily_change = STime().getCurrent();

    Json::Value json;
    BinaCPP::get_24hr(_connect_daily_change->getIdentifier().c_str(), json);

    BinanceErrorData error(json);
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return;
    }

    _connect_daily_change->set(BinancePriceStatisticsData(json));
}

bool BinanceController::getChart(ChartWrapper &wrapper, ChartInterval interval) const {
    const std::string& interval_converted = convertChartInterval(interval);
    if (interval_converted.empty()) {
        Logger::error("unknown chart interval");
        return false;
    }

    Json::Value json;
    BinaCPP::get_klines(wrapper.getIdentifier().c_str(), interval_converted.c_str(), 40, 0, 0, json);

    BinanceErrorData error(json);
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    if (not json.isArray()) {
        trace("%s\n", json.toStyledString().c_str());
        Logger::error("invalid chart json");
        return false;
    }

    for (Json::ArrayIndex i = 0; i < json.size(); ++i) {
        Json::Value& item = json[i];
        if (not wrapper.add(BinanceKlineData(item)))
            trace("skip kline: %s\n", item.toStyledString().c_str());
    }

    return true;
}

void BinanceController::connectChart(ChartWrapper& wrapper, ChartInterval interval) {
    if (not getChart(wrapper, interval))
        return;

    _connect_chart = &wrapper;

    const std::string& interval_converted = convertChartInterval(interval);
    const std::string& path = "/ws/" + util::lowercase(wrapper.getIdentifier().c_str()) + "@kline_" + interval_converted;
    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceController::onKlineDataStream, this, std::placeholders::_1), path.c_str());
}

int BinanceController::onKlineDataStream(Json::Value& json) {
    BinanceKlineData data(json);

    if (_connect_prices != nullptr) {
        _connect_prices->get(data.symbol)->add(data.price_close);
        _connect_prices->onChanged.emmit(data.symbol);
    }

    if (_connect_chart != nullptr) {
        if (not _connect_chart->add(data))
            trace("skip kline: %s\n", json.toStyledString().c_str());
    }
    return 0;
}

