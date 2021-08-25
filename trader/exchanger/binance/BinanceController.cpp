#include "BinanceController.hpp"
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"
#include "Config.hpp"
#include "Logger.hpp"
#include "util/StringUtil.hpp"
#include "util/NumberUtil.hpp"
#include "proxy/TraderTime.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/BookWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "response/BinanceEnums.hpp"
#include "response/BinanceErrorData.hpp"
#include "response/BinanceBalanceData.hpp"
#include "response/BinanceOrderData.hpp"
#include "response/BinancePriceStatisticsData.hpp"
#include "response/BinanceKlineData.hpp"

BinanceController::~BinanceController() {
    if (_thread.joinable())
        _thread.join();
}

bool BinanceController::init(const core::Config& config) {
    // init binance logger
    BinaCPP_logger::set_debug_level(0);
    BinaCPP_logger::enable_logfile(0);

    // init binance api
    static string api_key       = config.getAsString("BINANCE_API_KEY");
    static string secret_key    = config.getAsString("BINANCE_SECRET_KEY");
    if (api_key.empty() || secret_key.empty())
        return false;

    BinaCPP::init(api_key, secret_key);
    BinaCPP_websocket::init();

    return initUserListenKey();
}

void BinanceController::run() {
    _thread = std::thread(&BinaCPP_websocket::enter_event_loop);
}

void BinanceController::tick(time_t now) {
    auto timesup = [now](time_t time, time_t interval) {
        return time != 0 && now > time + interval;
    };

    // Keepalive a user data stream to prevent a time out.
    // User data streams will close after 60 minutes.
    // It's recommended to send a ping about every 30 minutes
    if (timesup(_time_keep_userstream, TraderTime::sMinute * 30))
        keepUserDataStream();

    // Update statistics
    if (timesup(_time_daily_change, TraderTime::sMinute * 15))
        updateDailyChange();

    // A single connection to stream.binance.com is only valid for 24 hours;
    // Expect to be disconnected at the 24 hour mark
    if (timesup(_time_start_userstream, TraderTime::sMinute * 1))
        startUserDataStream();

    if (timesup(_time_start_chart, TraderTime::sMinute * 1) && _chart_connector != nullptr)
        connectChart(*_chart_connector, ChartInterval::m15);
}

bool BinanceController::getSymbolInfo(Storage::Type_pair& container) const {
    Json::Value json;
    BinaCPP::get_exchangeInfo(json);

    BinanceErrorData error(json, "BinanceController::getSymbolInfo");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    const Json::Value& symbols = json["symbols"];
    if (not symbols.isArray()) {
        Logger::info("%s", json.toStyledString().c_str());
        Logger::error("BinanceController::getSymbolInfo: invalid exchange");
        return false;
    }

    _symbols.clear();
    for (uint i = 0; i < symbols.size(); ++i) {
        BinanceSymbolData data(symbols[i]);
        if (Symbol* symbol = container.get(data.symbol))
            symbol->set(data.baseAsset, data.quoteAsset);
        _symbols[data.symbol] = data;
    }

    return true;
}

bool BinanceController::getAllPrices(Storage::Type_price& container) const {
    Json::Value json;
    BinaCPP::get_allPrices(json);

    BinanceErrorData error(json, "BinanceController::getAllPrices");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    if (not json.isArray()) {
        Logger::info("%s\n", json.toStyledString().c_str());
        Logger::error("BinanceController::getAllPrices: invalid prices");
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

    _prices_connector = &container;
}

bool BinanceController::getBalances(Storage::Type_balance& container) const {
    Json::Value json;
    BinaCPP::get_account(BINANCE_RECV_WINDOW, json);

    BinanceErrorData error(json, "BinanceController::getBalances");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    const Json::Value& balances = json["balances"];
    if (not balances.isArray()) {
        Logger::info("%s", json.toStyledString().c_str());
        Logger::error("BinanceController::getBalances: invalid account");
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

    _balances_connector = &container;
    startUserDataStream();
}

bool BinanceController::initUserListenKey() {
    Json::Value json;

    BinaCPP::start_userDataStream(json);
    BinanceErrorData error(json, "BinanceController::initUserListenKey");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    if (!json["listenKey"] || !json["listenKey"].isString()) {
        Logger::info("%s", json.toStyledString().c_str());
        Logger::error("BinanceController::initUserListenKey: can't get listenKey for stream account");
        return false;
    }

    _stream_listen_key = json["listenKey"].asString();
    return true;
}

void BinanceController::startUserDataStream() {
    std::string ws_path = std::string("/ws/");
    ws_path.append(_stream_listen_key);

    _time_start_userstream = 0;
    _time_keep_userstream = Time().ms();
    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceController::onUserDataStream, this, std::placeholders::_1), ws_path.c_str());
}

void BinanceController::keepUserDataStream() {
    if (_stream_listen_key.empty())
        return;

    _time_keep_userstream = Time().ms();
    BinaCPP::keep_userDataStream(_stream_listen_key.c_str());
}

int BinanceController::onUserDataStream(Json::Value &json) {
    BinanceErrorData error(json, "BinanceController::onUserDataStream");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        if (error.code == BinanceErrorData::DISCONNECTED) {
            _time_start_userstream = Time().ms();
            _time_keep_userstream = 0;
        }
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
            if (_balances_connector != nullptr) {
                _balances_connector->get(data.asset)->set(data.free, data.locked);
                _balances_connector->onChanged.emmit(data.asset);
            }
        }
    }

    return 0;
}

const CandlestickWrapper* BinanceController::connectStats(CandlestickWrapper& wrapper) {
    _stats_connector = &wrapper;
    updateDailyChange();
    return _stats_connector;
}

void BinanceController::updateDailyChange() {
    if (_stats_connector == nullptr)
        return;

    _time_daily_change = Time().ms();

    Json::Value json;
    BinaCPP::get_24hr(_stats_connector->id().c_str(), json);

    BinanceErrorData error(json, "BinanceController::updateDailyChange");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return;
    }

    _stats_connector->set(BinancePriceStatisticsData(json));
}

bool BinanceController::getChart(ChartWrapper &wrapper, ChartInterval interval) const {
    const std::string& interval_converted = binance::serialize(interval);
    if (interval_converted.empty()) {
        Logger::error("BinanceController::getChart: unknown chart interval");
        return false;
    }

    Json::Value json;
    BinaCPP::get_klines(wrapper.id().c_str(), interval_converted.c_str(), 40, 0, 0, json);

    BinanceErrorData error(json, "BinanceController::getChart");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    if (not json.isArray()) {
        Logger::info("%s", json.toStyledString().c_str());
        Logger::error("BinanceController::getChart: invalid chart json");
        return false;
    }

    for (Json::ArrayIndex i = 0; i < json.size(); ++i) {
        Json::Value& item = json[i];
        if (wrapper.add(BinanceKlineData(item)) == nullptr)
            Logger::info("BinanceController::getChart: invalid kline %s", item.toStyledString().c_str());
    }

    return true;
}

const ChartWrapper* BinanceController::connectChart(ChartWrapper& wrapper, ChartInterval interval) {
    _chart_connector = &wrapper;
    const std::string& path = "/ws/" + util::lowercase(wrapper.id().c_str()) + "@kline_" + binance::serialize(interval);
    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceController::onKlineDataStream, this, std::placeholders::_1), path.c_str());
    return _chart_connector;
}

int BinanceController::onKlineDataStream(Json::Value& json) {
    BinanceErrorData error(json, "BinanceController::onKlineDataStream");
    if (error.has()) {
        Logger::error(error.msg.c_str());
	if (error.code == BinanceErrorData::DISCONNECTED) {
            _time_start_chart = Time().ms();
        }
        return 0;
    }

    BinanceKlineData data(json);

    if (_prices_connector != nullptr) {
        _prices_connector->get(data.symbol)->add(data.price_close);
        _prices_connector->onChanged.emmit(data.symbol);
    }

    if (_chart_connector != nullptr) {
        if (_chart_connector->add(data) == nullptr)
            Logger::info("BinanceController::getOrders: skip kline %s", json.toStyledString().c_str());
    }
    return 0;
}

bool BinanceController::getOrders(BookWrapper& wrapper) const {
    Json::Value json;
    BinaCPP::get_allOrders(wrapper.id().c_str(), 0, 0, BINANCE_RECV_WINDOW, json);

    std::vector<BinanceOrderData> vec;

    BinanceErrorData error(json, "BinanceController::getOrders");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    if (not json.isArray()) {
        Logger::info("%s", json.toStyledString().c_str());
        Logger::error("BinanceController::getOrders: invalid orders");
        return false;
    }

    for (uint i = 0; i < json.size(); ++i) {
        Json::Value item = json[i];
        if (not wrapper.add(BinanceOrderData(json[i], false)))
            Logger::info("BinanceController::getOrders: invalid order: %s", item.toStyledString().c_str());
    }

    return true;
}

const BookWrapper* BinanceController::connectOrders(BookWrapper& wrapper) {
    if (not getOrders(wrapper))
        return nullptr;

    _orders_connector = &wrapper;
    return _orders_connector;
}

const OrderWrapper* BinanceController::createOrder(const OrderRequest& request) {
    if (_orders_connector == nullptr) {
        Logger::error("BinanceController::createOrder orders not connected");
        return nullptr;
    }

    std::string symbol = _orders_connector->id();
    std::string type = binance::serialize(request.type);
    Quantity quantity = request.quantity;

    auto it = _symbols.find(symbol);
    if (it == _symbols.end()) {
        Logger::error("BinanceController::createOrder unknown symbol");
        return nullptr;
    }

    const BinanceSymbolData& info = it->second;
    if (not info.hasOrderType(type)) {
        Logger::error("BinanceController::createOrder symbol doesn't supported for this type");
        return nullptr;
    }

    Quantity min_quantity = getMinQuantity(info);
    if (min_quantity == 0.0) {
        Logger::error("BinanceController::createOrder unknown min quantity");
        return nullptr;
    } else if (quantity > min_quantity) {
//fixme
//        const BinanceSymbolData::LotSize& lot_size = info.lotSize;
//        if (info.lotSize.stepSize > 0.0 && quantity != util::ceil_steps(quantity, info.lotSize.stepSize)) {
//            Logger::error("BinanceController::createOrder quantity not equal step size");
//            return nullptr;
//        }
    } else {
        quantity = getMinQuantity(info, 1.2);
    }

    Json::Value json;
    BinaCPP::send_order(symbol.c_str(), binance::serialize(request.side).c_str(), binance::serialize(request.type).c_str(), "GTC", quantity , 0, "", 0, 0, BINANCE_TEST_MODE, BINANCE_RECV_WINDOW, json);

    BinanceErrorData error(json, "BinanceController::createOrder");
    if (error.has()) {
        Logger::info("%s", error.msg.c_str());
        return nullptr;
    }

    BinanceOrderData data(json, false);
    if (data.status.empty()) {
        Logger::error("BinanceController::createOrder empty response");
        return nullptr;
    }

    return _orders_connector->add(data);
}

double BinanceController::getMinQuantity(const BinanceSymbolData& info, double multiplier/* = 1.2*/) const {
    if (_prices_connector == nullptr)
        return 0.0;

    const PriceWrapper* wrapper = _prices_connector->get(info.symbol);
    if (wrapper == nullptr)
        return 0.0;

    const BinanceSymbolData::MinNotional& min_notional = info.minNotional;
    const BinanceSymbolData::LotSize& lot_size = info.lotSize;

    Price price_avg = wrapper->getPriceAverage(min_notional.avgPriceMins * TraderTime::sMinute);
    double quantity = std::max(lot_size.minQty, min_notional.minNotional / price_avg) *  multiplier;
    if (info.lotSize.stepSize > 0.0)
        quantity = util::ceil_steps(quantity, info.lotSize.stepSize);
    return quantity;
}

std::unordered_map<std::string, BinanceSymbolData> BinanceController::_symbols;
