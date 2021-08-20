#include "BinanceController.hpp"
#include "BinanceWebsocket.hpp"
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"
#include "Config.hpp"
#include "Logger.hpp"
#include "util/StringUtil.hpp"
#include "util/NumberUtil.hpp"
#include "Time.hpp"
#include "exchanger/base/Symbol.hpp"
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

static const char* CONFIG_API_KEY = "BINANCE_API_KEY";
static const char* CONFIG_SECRET_KEY = "BINANCE_SECRET_KEY";
static const char* CONFIG_RECV_WINDOW = "BINANCE_RECV_WINDOW";
static const char* CONFIG_TEST_MODE = "BINANCE_TEST_MODE";

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

    // init binance api
    static string api_key       = config.getAsString(CONFIG_API_KEY);
    static string secret_key    = config.getAsString(CONFIG_SECRET_KEY);
    if (api_key.empty() || secret_key.empty())
        return false;

    if (config.has(CONFIG_RECV_WINDOW))
        _config_recv_window = config.getAsInt(CONFIG_RECV_WINDOW);

    if (config.has(CONFIG_TEST_MODE))
        _config_test_mode = config.getAsInt(CONFIG_TEST_MODE);

    BinaCPP::init(api_key, secret_key);
    BinaCPP_websocket::init();

    return initUserListenKey();
}

void BinanceController::run() {
    // first tick connecting websockets
    tick(Time().ms());
    // listen websockets
    _thread = std::thread(&BinaCPP_websocket::enter_event_loop);
}

void BinanceController::tick(time_t now) {
    // keep websockets connctions
    for (BinanceWebsocket* websocket : _websockets) {
        websocket->connect();
    }

    auto timesup = [now](time_t time, time_t interval) {
        return time != 0 && now > time + interval;
    };

    // Keepalive a user data stream to prevent a time out.
    // User data streams will close after 60 minutes.
    // It's recommended to send a ping about every 30 minutes
    if (timesup(_time_keep_userstream, TraderTime::sMinute * 30))
        keepUserDataStream();
}

bool BinanceController::loadPairs(Storage::Type_pair& container) const {
    Json::Value json;
    BinaCPP::get_exchangeInfo(json);

    BinanceErrorData error(json, "BinanceController::loadPairs");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    const Json::Value& symbols = json["symbols"];
    if (not symbols.isArray()) {
        Logger::info("%s", json.toStyledString().c_str());
        Logger::error("BinanceController::loadPairs: invalid exchange");
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

bool BinanceController::loadPrices(Storage::Type_price& container) const {
    Json::Value json;
    BinaCPP::get_allPrices(json);

    BinanceErrorData error(json, "BinanceController::loadPrices");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    if (not json.isArray()) {
        Logger::info("%s\n", json.toStyledString().c_str());
        Logger::error("BinanceController::loadPrices: invalid prices");
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

bool BinanceController::loadBalances(Storage::Type_balance& container) const {
    Json::Value json;
    BinaCPP::get_account(_config_recv_window, json);

    BinanceErrorData error(json, "BinanceController::loadBalances");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    const Json::Value& balances = json["balances"];
    if (not balances.isArray()) {
        Logger::info("%s", json.toStyledString().c_str());
        Logger::error("BinanceController::loadBalances: invalid account");
        return false;
    }

    for (uint i = 0; i < balances.size(); ++i) {
        BinanceBalanceData data(balances[i], false);
        container.get(data.asset)->set(data.free, data.locked);
    }
    return true;
}

bool BinanceController::loadStats(CandlestickWrapper& container) const {
    Json::Value json;
    BinaCPP::get_24hr(container.id().c_str(), json);

    BinanceErrorData error(json, "BinanceController::loadStats");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    container.set(BinancePriceStatisticsData(json));
    return true;
}

bool BinanceController::loadCharts(ChartWrapper& container, ChartInterval interval) const {
    const std::string& interval_converted = binance::serialize(interval);
    if (interval_converted.empty()) {
        Logger::error("BinanceController::loadCharts: unknown chart interval");
        return false;
    }

    Json::Value json;
    BinaCPP::get_klines(container.id().c_str(), interval_converted.c_str(), 40, 0, 0, json);

    BinanceErrorData error(json, "BinanceController::loadCharts");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    if (not json.isArray()) {
        Logger::info("%s", json.toStyledString().c_str());
        Logger::error("BinanceController::loadCharts: invalid chart json");
        return false;
    }

    for (Json::ArrayIndex i = 0; i < json.size(); ++i) {
        Json::Value& item = json[i];
        if (container.add(BinanceKlineData(item)) == nullptr)
            Logger::info("BinanceController::loadCharts: invalid kline %s", item.toStyledString().c_str());
    }

    return true;
}

bool BinanceController::loadOrders(BookWrapper& container) const {
    Json::Value json;
    BinaCPP::get_allOrders(container.id().c_str(), 0, 0, _config_recv_window, json);

    std::vector<BinanceOrderData> vec;

    BinanceErrorData error(json, "BinanceController::loadOrders");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        return false;
    }

    if (not json.isArray()) {
        Logger::info("%s", json.toStyledString().c_str());
        Logger::error("BinanceController::loadOrders: invalid orders");
        return false;
    }

    for (uint i = 0; i < json.size(); ++i) {
        Json::Value item = json[i];
        if (not container.add(BinanceOrderData(json[i], false)))
            Logger::info("BinanceController::loadOrders: invalid order: %s", item.toStyledString().c_str());
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

    _time_keep_userstream = Time().ms();
    BinaCPP::keep_userDataStream(userstream->path().c_str());
}

void BinanceController::onUserDataStream(const Json::Value& json) {
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
            if (_balances_connector != nullptr)
                _balances_connector->get(data.asset)->set(data.free, data.locked);
        }
    }
}

void BinanceController::onKlineDataStream(const Json::Value& json) {
    BinanceKlineData data(json);

    if (_prices_connector != nullptr)
        _prices_connector->get(data.symbol)->add(data.price_close);

    if (_charts_connector != nullptr)
        _charts_connector->get(data.symbol)->add(data);
}

const OrderWrapper* BinanceController::createOrder(BookWrapper& container, const OrderRequest& request) {
    std::string type = binance::serialize(request.type);
    Quantity quantity = request.quantity;

    auto it = _symbols.find(request.symbol);
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
        quantity = min_quantity;
    }

    Json::Value json;
    BinaCPP::send_order(request.symbol.c_str(), binance::serialize(request.side).c_str(), binance::serialize(request.type).c_str(), "GTC", quantity , 0, "", 0, 0, _config_test_mode, _config_recv_window, json);

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

    return container.add(data);
}

double BinanceController::getMinQuantity(const BinanceSymbolData& info) const {
    if (_prices_connector == nullptr)
        return 0.0;

    const PriceWrapper* wrapper = _prices_connector->get(info.symbol);
    if (wrapper == nullptr)
        return 0.0;

    const BinanceSymbolData::MinNotional& min_notional = info.minNotional;
    const BinanceSymbolData::LotSize& lot_size = info.lotSize;

    Price price_avg = wrapper->getPriceAverage(min_notional.avgPriceMins * TraderTime::sMinute);
    double quantity = std::max(lot_size.minQty, min_notional.minNotional / price_avg) *  1.2;
    if (info.lotSize.stepSize > 0.0)
        quantity = util::ceil_steps(quantity, info.lotSize.stepSize);
    return quantity;
}

std::unordered_map<std::string, BinanceSymbolData> BinanceController::_symbols;
