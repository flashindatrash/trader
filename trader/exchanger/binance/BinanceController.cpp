#include "BinanceController.hpp"
#include "BinanceWebsocket.hpp"
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"
#include "Config.hpp"
#include "Logger.hpp"
#include "util/StringUtil.hpp"
#include "Time.hpp"
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
#include "response/BinanceTickerData.hpp"

static const char* CONFIG_API_KEY = "BINANCE_API_KEY";
static const char* CONFIG_SECRET_KEY = "BINANCE_SECRET_KEY";
static const char* CONFIG_RECV_WINDOW = "BINANCE_RECV_WINDOW";
static const char* CONFIG_FEE = "BINANCE_FEE";

BinanceController::~BinanceController() {
    for (BinanceWebsocket* websocket : _websockets)
        SAFE_DELETE(websocket);
    _websockets.clear();

    BinaCPP_websocket::exit_event_loop();

    if (_thread.joinable())
        _thread.join();
}

bool BinanceController::init(const core::Config& config) {
    // init binance logger
    BinaCPP_logger::set_debug_level(0);
    BinaCPP_logger::enable_logfile(0);

    // init binance config
    static string api_key       = config.asString(CONFIG_API_KEY);
    static string secret_key    = config.asString(CONFIG_SECRET_KEY);
    if (api_key.empty() || secret_key.empty())
        return false;

    if (config.has(CONFIG_RECV_WINDOW))
        _config_recv_window = config.asInt(CONFIG_RECV_WINDOW);

    if (config.has(CONFIG_FEE))
        _config_fee = config.asDouble(CONFIG_FEE) / 100.0;

    // init binance api
    if (not BinaCPP::init(api_key, secret_key))
        return false;

    // init binance websocket
    if (not BinaCPP_websocket::init())
        return false;

    return initUserListenKey();
}

void BinanceController::run() {
    // first tick connecting websockets
    tick(Time().ms());
    // listen websockets
    _thread = std::thread(&BinaCPP_websocket::enter_event_loop);
}

void BinanceController::tick(time_t now) {
    // keep websockets connections
    for (BinanceWebsocket* websocket : _websockets) {
        websocket->connect();
    }

    // Keepalive a user data stream to prevent a timeout.
    // User data streams will close after 60 minutes.
    // It's recommended to send a ping about every 30 minutes
    if (_time_keep_userstream == 0)
        _time_keep_userstream = Time().ms();

    if (now > _time_keep_userstream + Timer::sMinute * 30) {
        keepUserDataStream();
        _time_keep_userstream = Time().ms();
    }
}

bool BinanceController::loadPairs(Storage::Type_pair& container) const {
    Json::Value json;
    BinaCPP::get_exchangeInfo(json);

    BinanceErrorData error(json, "BinanceController::loadPairs");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    const Json::Value& symbols = json["symbols"];
    if (not symbols.isArray()) {
        Logger::info(util::format("BinanceController::loadPairs: invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    _symbols.clear();
    for (const auto & it : symbols) {
        BinanceSymbolData data(it);
        if (Symbol* symbol = container.get(data.symbol))
            symbol->set(data.baseAsset, data.quoteAsset);
        _symbols[data.symbol] = data;
    }

    return true;
}

bool BinanceController::loadPrices(Storage::Type_price& container) const {
    Json::Value json;
    BinaCPP::get_allPrices(json);

    BinanceErrorData error(json, "BinanceController::loadPrices");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    if (not json.isArray()) {
        Logger::info(util::format("BinanceController::loadPrices: invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (auto & data : json) {
        std::string symbol = data["symbol"].asString();
        Price price = atof(data["price"].asString().c_str());
        container.get(symbol)->set(price);
    }

    return true;
}

bool BinanceController::loadBalances(Storage::Type_balance& container) const {
    Json::Value json;
    BinaCPP::get_account(_config_recv_window, json);

    BinanceErrorData error(json, "BinanceController::loadBalances");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    const Json::Value& balances = json["balances"];
    if (not balances.isArray()) {
        Logger::info(util::format("BinanceController::loadBalances: invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (const auto & balance : balances) {
        BinanceBalanceData data(balance, false);
        container.get(data.asset)->set(data.free, data.locked);
    }
    return true;
}

bool BinanceController::loadStats(CandlestickWrapper& container) const {
    Json::Value json;
    BinaCPP::get_24hr(container.id().c_str(), json);

    BinanceErrorData error(json, "BinanceController::loadStats");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    container.set(BinancePriceStatisticsData(json));
    return true;
}

bool BinanceController::loadCharts(ChartWrapper& container, ChartRequest& request) const {
    const std::string& interval_converted = binance::serialize(request.interval);
    if (interval_converted.empty()) {
        Logger::info("BinanceController::loadCharts: unknown chart interval");
        return false;
    }

    Json::Value json;
    BinaCPP::get_klines(container.id().c_str(), interval_converted.c_str(), 0, request.time_start, request.time_end, json);

    BinanceErrorData error(json, "BinanceController::loadCharts");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    if (not json.isArray()) {
        Logger::info(util::format("BinanceController::loadCharts: invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (auto& item : json) {
        BinanceKlineData data(item);
        data.symbol = container.id();

        if (container.add(data) == nullptr)
            Logger::info(util::format("BinanceController::loadCharts: invalid kline %s", item.toStyledString().c_str()));
    }

    return true;
}

bool BinanceController::loadOrders(BookWrapper& container) const {
    Json::Value json;
    BinaCPP::get_allOrders(container.id().c_str(), 0, 0, _config_recv_window, json);

    std::vector<BinanceOrderData> vec;

    BinanceErrorData error(json, "BinanceController::loadOrders");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    if (not json.isArray()) {
        Logger::info(util::format("BinanceController::loadOrders: invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (const auto& item : json) {
        if (not container.add(BinanceOrderData(item, false)))
            Logger::info(util::format("BinanceController::loadOrders: invalid order: %s", item.toStyledString().c_str()));
    }

    return true;
}

void BinanceController::connectPrices(Storage::Type_price& container) {
    loadPrices(container);
    _prices_connector = &container;
}

void BinanceController::connectBalances(Storage::Type_balance& container) {
    loadBalances(container);
    _balances_connector = &container;
}

void BinanceController::connectCharts(Storage::Type_chart& container) {
    _charts_connector = &container;
}

void BinanceController::listenCharts(ChartWrapper& container, ChartInterval interval) {
    BinanceWebsocket* websocket = BinanceWebsocket::create();
    websocket->setPath(util::lowercase(container.id().c_str()) + "@kline_" + binance::serialize(interval));
    websocket->setCallback(std::bind(&BinanceController::onKlineDataStream, this, std::placeholders::_1));
    _websockets.push_back(websocket);
}

void BinanceController::listenTicker(PriceWrapper& container) {
    BinanceWebsocket* websocket = BinanceWebsocket::create();
    websocket->setPath(util::lowercase(container.id().c_str()) + "@bookTicker");
    websocket->setCallback(std::bind(&BinanceController::onTickerDataStream, this, std::placeholders::_1));
    _websockets.push_back(websocket);
}

bool BinanceController::initUserListenKey() {
    Json::Value json;

    BinaCPP::start_userDataStream(json);
    BinanceErrorData error(json, "BinanceController::initUserListenKey");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    if (!json["listenKey"] || !json["listenKey"].isString()) {
        Logger::info(util::format("%s", json.toStyledString().c_str()));
        Logger::info("BinanceController::initUserListenKey: can't get listenKey for stream account");
        return false;
    }

    BinanceWebsocket* websocket = BinanceWebsocket::create();
    websocket->setPath(json["listenKey"].asString());
    websocket->setCallback(std::bind(&BinanceController::onUserDataStream, this, std::placeholders::_1));
    _websockets.push_back(websocket);
    return true;
}

void BinanceController::keepUserDataStream() {
    if (_websockets.empty())
        return;

    BinanceWebsocket* userstream = _websockets.front();
    if (not userstream->isConnected())
        return;

    BinaCPP::keep_userDataStream(userstream->path().c_str());
}

void BinanceController::onUserDataStream(const Json::Value& json) {
    std::string action = json["e"].asString();
    if (action  == "executionReport") {
        std::string executionType = json["x"].asString();
        if (executionType == "NEW") {
            BinanceOrderData order(json, true);
            if (order.isRejected())
                Logger::info(util::format("Order rejected %s", json["r"].asString().c_str()));
        }
    } else if (action == "outboundAccountPosition") {
        for (const auto &i : json["B"]) {
            BinanceBalanceData data(i, true);
            if (_balances_connector != nullptr)
                _balances_connector->get(data.asset)->set(data.free, data.locked);
        }
    }
}

void BinanceController::onKlineDataStream(const Json::Value& json) {
    BinanceKlineData data(json);

    if (_prices_connector != nullptr)
        _prices_connector->get(data.symbol)->set(data.price_close);

    if (_charts_connector != nullptr)
        _charts_connector->get(data.symbol)->add(data);
}

void BinanceController::onTickerDataStream(const Json::Value& json) {
    BinanceTickerData data(json);

    if (_prices_connector != nullptr)
        _prices_connector->get(data.symbol)->set(data);
}

const OrderWrapper* BinanceController::createOrder(BookWrapper& container, OrderRequest& request) {
    if (request.side == OrderSide::Invalid)
        return nullptr;

    std::string type = binance::serialize(request.type);

    auto it = _symbols.find(request.symbol);
    if (it == _symbols.end()) {
        Logger::info("BinanceController::createOrder unknown symbol");
        return nullptr;
    }

    const BinanceSymbolData& info = it->second;
    if (not info.hasOrderType(type)) {
        Logger::info("BinanceController::createOrder symbol doesn't supported for this type");
        return nullptr;
    }

    // round quantity to step size and min lot
    request.quantity = roundQuantity(request.quantity, request.symbol);

    // enough to create order
    if (not request.isEnough())
        return nullptr;

    Json::Value json;
    BinaCPP::send_order(request.symbol.c_str(), binance::serialize(request.side).c_str(), type.c_str(), "GTC", request.quantity , 0, "", 0, 0, _config_recv_window, json);

    // check errors
    BinanceErrorData error(json, "BinanceController::createOrder");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        if (error.code == BinanceErrorData::NEW_ORDER_REJECTED) {
            Logger::info(util::format("BinanceController::create order: not enough to %s %f %s with balance %f",
                         binance::serialize(request.side).c_str(),
                         request.quantity,
                         request.symbol.baseAsset().c_str(),
                         OrderUtil::usedQuantity(request.side, request.symbol.baseAsset().balance(),
                                                 request.symbol.quoteAsset().balance())));

            Logger::error("");
        }
        return nullptr;
    }

    // parse binance structure
    BinanceOrderData data(json, false);
    if (data.status.empty()) {
        Logger::info("BinanceController::createOrder empty response");
        return nullptr;
    }

    // create & add to container
    const OrderWrapper* wrapper = container.add(data);
    if (wrapper == nullptr)
        return nullptr;

    // update balance
    wrapper->operate();
    return wrapper;
}

double BinanceController::minQuantity(const std::string& symbol) const {
    auto it = _symbols.find(symbol);
    if (it == _symbols.end())
        return 0.0;

    const BinanceSymbolData& info = it->second;
    if (_prices_connector == nullptr)
        return 0.0;

    const PriceWrapper* wrapper = _prices_connector->get(info.symbol);
    if (wrapper == nullptr)
        return 0.0;

    const BinanceSymbolData::MinNotional& min_notional = info.minNotional;
    const BinanceSymbolData::LotSize& lot_size = info.lotSize;

    // todo: price average
    // Price price_avg = wrapper->getPriceAverage(min_notional.avgPriceMins * Timer::sMinute);
    Price price_avg = wrapper->get();
    double quantity = std::max(lot_size.minQty, min_notional.minNotional / price_avg) *  1.3;
    if (info.lotSize.stepSize > 0.0)
        quantity = std::round(quantity / info.lotSize.stepSize) * info.lotSize.stepSize;
    return quantity;
}

double BinanceController::roundQuantity(double quantity, const std::string& symbol) const {
    auto it = _symbols.find(symbol);
    if (it == _symbols.end())
        return 0.0;

    const BinanceSymbolData& info = it->second;

    if (info.lotSize.stepSize > 0.0)
        quantity = std::round(quantity / info.lotSize.stepSize) * info.lotSize.stepSize;

    return std::max(quantity, minQuantity(symbol));
}

double BinanceController::fee() const {
    return _config_fee;
}

std::unordered_map<std::string, BinanceSymbolData> BinanceController::_symbols;
