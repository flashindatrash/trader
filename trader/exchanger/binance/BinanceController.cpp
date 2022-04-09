#include "BinanceController.hpp"
#include "BinanceWebsocket.hpp"
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
#include "response/BinanceFlexibleProductData.hpp"
#include "response/BinanceSpotAccountData.hpp"
#include "response/BinancePriceData.hpp"
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"
#include "core/Config.hpp"
#include "core/Logger.hpp"
#include "core/Time.hpp"
#include <cmath>
#include <algorithm>

static const char* CONFIG_API_KEY = "BINANCE_API_KEY";
static const char* CONFIG_SECRET_KEY = "BINANCE_SECRET_KEY";
static const char* CONFIG_RECV_WINDOW = "BINANCE_RECV_WINDOW";

double BinanceController::_commission = 0.0;

BinanceController::~BinanceController() {
    for (BinanceWebsocket* websocket : _websockets)
        delete websocket;
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
        if (keepUserDataStream())
            _time_keep_userstream = Time().ms();
    }
}

bool BinanceController::loadPairs(Storage::Type_pair& container) const {
    if (not checkRateLimits())
        return false;

    Json::Value json;
    BinaCPP::get_exchangeInfo(json);

    BinanceErrorData error(json, "BinanceController::loadPairs");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    BinanceExchangeData data(json);
    if (data.symbols.empty()) {
        Logger::info(util::format("BinanceController::loadPairs: invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    _exchange_info = std::move(data);
    for (auto& it : _exchange_info.symbols) {
        if (Symbol* symbol = container.get(it.first)) {
            if (not symbol->baseAsset().empty() && not symbol->quoteAsset().empty())
                continue;

            symbol->set(it.second.baseAsset, it.second.quoteAsset);
            Symbol::onAdded.emmit(*symbol);
        }
    }

    return true;
}

bool BinanceController::loadPrices(Storage::Type_price& container) const {
    if (not checkRateLimits())
        return false;

    Json::Value json;
    BinaCPP::get_prices("", json);

    BinanceErrorData error(json, "BinanceController::loadPrices");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    if (not json.isArray()) {
        Logger::info(util::format("BinanceController::loadPrices: invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (auto & it : json) {
        BinancePriceData data(it);
        container.get(data.symbol)->set(data.price);
    }

    return true;
}

bool BinanceController::loadPrice(PriceWrapper& container) const {
    if (not checkRateLimits())
        return false;

    Json::Value json;
    BinaCPP::get_prices(container.id().c_str(), json);

    BinanceErrorData error(json, "BinanceController::loadPrice");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    if (not json.isArray()) {
        Logger::info(util::format("BinanceController::loadPrice: invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (auto & it : json) {
        BinancePriceData data(it);
        if (container.id() != data.symbol) {
            Logger::info(util::format("BinanceController::loadPrice: invalid container %s for symbol %s", container.id().c_str(), data.symbol.c_str()));
            return false;
        }

        container.set(data.price);
    }

    return true;
}

bool BinanceController::loadBalances(Storage::Type_balance& container) const {
    if (not checkRateLimits())
        return false;

    Json::Value json;
    BinaCPP::get_account(_config_recv_window, json);

    BinanceErrorData error(json, "BinanceController::loadBalances");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    BinanceSpotAccountData account(json);
    if (not account.canTrade) {
        Logger::info("BinanceController::loadBalances: can't trade on spot account");
        return false;
    }

    _commission = account.takerCommission / 100.0;

    for (const BinanceBalanceData& balance : account.balances)
        container.get(balance.asset)->set(balance.free, balance.locked);

    return true;
}

bool BinanceController::loadSavings(Storage::Type_balance& container) const {
    if (not checkRateLimits())
        return false;

    Json::Value json;
    BinaCPP::get_savings(_config_recv_window, json);

    BinanceErrorData error(json, "BinanceController::loadSavings");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    const Json::Value& balances = json["positionAmountVos"];
    if (not balances.isArray()) {
        Logger::info(util::format("BinanceController::loadSavings: invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (const auto & balance : balances) {
        BinanceBalanceData data(balance, "asset", "amount");
        container.get("LD" + data.asset)->set(data.free, data.locked);
    }

    return true;
}

bool BinanceController::redeemSavings(const std::string& asset, double quantity) const {
    if (not checkRateLimits())
        return false;

    if (_balances_connector != nullptr && _balances_connector->get("LD" + asset)->get() < quantity)
        return false;

    Json::Value json;
    BinaCPP::get_flexibleProducts(asset.c_str(), _config_recv_window, json);

    BinanceErrorData error(json, "BinanceController::redeemSavings");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        return false;
    }

    if (not json.isArray()) {
        Logger::info(util::format("BinanceController::redeemSavings: invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (const auto & product : json) {
        BinanceFlexibleProductData data(product);

        if (not data.canRedeem || data.free < quantity || data.redeemingAmount > 0.0)
            continue;

        Json::Value json_redeem;
        BinaCPP::redeem_flexibleProduct(data.productId.c_str(), quantity, "FAST", _config_recv_window, json_redeem);

        BinanceErrorData error_redeem(json, "BinanceController::redeemSavings");
        if (error_redeem.has()) {
            Logger::info(util::format("%s [%d]", error_redeem.msg.c_str(), error_redeem.code));
            return false;
        }

        return true;
    }

    return false;
}

bool BinanceController::loadStats(CandlestickWrapper& container) const {
    if (not checkRateLimits())
        return false;

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
    if (not checkRateLimits())
        return false;

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
    if (not checkRateLimits())
        return false;

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
    loadSavings(container);
    _balances_connector = &container;
}

void BinanceController::connectCharts(Storage::Type_chart& container) {
    _charts_connector = &container;
}

void BinanceController::listenCharts(ChartWrapper& container, ChartInterval interval) {
    const std::string& path = util::lowercase(container.id().c_str()) + "@kline_" + binance::serialize(interval);
    if (findWebsocket(path) != _websockets.end())
        return;

    BinanceWebsocket* websocket = BinanceWebsocket::create();
    websocket->setPath(path);
    websocket->setCallback(std::bind(&BinanceController::onKlineDataStream, this, std::placeholders::_1));
    _websockets.push_back(websocket);
}

void BinanceController::listenTicker(PriceWrapper& container) {
    const std::string& path = util::lowercase(container.id().c_str()) + "@bookTicker";
    if (findWebsocket(path) != _websockets.end())
        return;

    BinanceWebsocket* websocket = BinanceWebsocket::create();
    websocket->setPath(path);
    websocket->setCallback(std::bind(&BinanceController::onTickerDataStream, this, std::placeholders::_1));
    _websockets.push_back(websocket);
}

void BinanceController::unlistenTicker(PriceWrapper& container) {
    const std::string &path = util::lowercase(container.id().c_str()) + "@bookTicker";
    auto it = findWebsocket(path);
    if (it == _websockets.end())
        return;

    BinanceWebsocket* websocket = *it;
    if (websocket->disconnect()) {
        _websockets.erase(it);
        delete websocket;
    }
}

bool BinanceController::initUserListenKey() {
    if (not checkRateLimits())
        return false;

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
    websocket->setType(BinanceWebsocket::UserStream);
    websocket->setCallback(std::bind(&BinanceController::onUserDataStream, this, std::placeholders::_1));
    _websockets.push_back(websocket);
    return true;
}

bool BinanceController::keepUserDataStream() {
    if (not checkRateLimits())
        return false;

    auto it = std::find_if(_websockets.begin(), _websockets.end(), [](BinanceWebsocket* i) { return i->type() == BinanceWebsocket::UserStream; });
    if (it == _websockets.end())
        return false;

    BinanceWebsocket* websocket = *it;
    if (not websocket->isConnected())
        BinaCPP::keep_userDataStream(websocket->path().c_str());

    return websocket->isConnected();
}

std::vector<BinanceWebsocket*>::iterator BinanceController::findWebsocket(const std::string& path) {
    return std::find_if(_websockets.begin(), _websockets.end(), [path](BinanceWebsocket* i) { return i->path() == path; });
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
            BinanceBalanceData data(i, "a", "f", "l");
            Logger::info(util::format("Balance %s: %f", data.asset.c_str(), data.free));
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

    //Logger::info(util::format("bestAskPrice: %f", data.bestAskPrice));

    if (_prices_connector != nullptr)
        _prices_connector->get(data.symbol)->set(data);
}

const OrderWrapper* BinanceController::createOrder(BookWrapper& container, OrderRequest& request) {
    if (not checkRateLimits())
        return nullptr;

    if (request.side == OrderSide::Invalid)
        return nullptr;

    std::string type = binance::serialize(request.type);

    auto it = _exchange_info.symbols.find(request.symbol);
    if (it == _exchange_info.symbols.end()) {
        Logger::info("BinanceController::createOrder unknown symbol");
        return nullptr;
    }

    const BinanceSymbolData& info = it->second;
    if (not info.hasOrderType(type)) {
        Logger::info("BinanceController::createOrder symbol doesn't supported for this type");
        return nullptr;
    }

    if (not info.isSpotTradingAllowed) {
        Logger::info("BinanceController::createOrder spot trading is not allowed");
        return nullptr;
    }

    // round quantity to step size and min lot
    request.quantity = roundQuantity(request.quantity, request.symbol, std::round);

    // check is enough to create order
    const Asset& asset = OrderUtil::usedAsset(request.side, request.symbol);
    if (asset.balance() < request.required()) {
        // try to redeem from savings
        if (not redeemSavings(asset, request.required() - asset.balance()))
            return nullptr;

        // check one more time after redeeming
        if (asset.balance() < request.required())
            return nullptr;
    }

    Json::Value json;
    BinaCPP::send_order(request.symbol.c_str(), binance::serialize(request.side).c_str(), type.c_str(), "GTC", request.quantity , 0, "", 0, 0, _config_recv_window, json);

    // check errors
    BinanceErrorData error(json, "BinanceController::createOrder");
    if (error.has()) {
        Logger::info(util::format("%s [%d]", error.msg.c_str(), error.code));
        if (error.code == BinanceErrorData::NEW_ORDER_REJECTED) {
            Logger::error(util::format("BinanceController::create order: not enough to %s %f %s with balance %f",
                         binance::serialize(request.side).c_str(),
                         request.quantity,
                         request.symbol.baseAsset().c_str(),
                         OrderUtil::usedAsset(request.side, request.symbol).balance()));
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
    auto it = _exchange_info.symbols.find(symbol);
    if (it == _exchange_info.symbols.end())
        return 0.0;

    const BinanceSymbolData& info = it->second;
    if (_prices_connector == nullptr)
        return 0.0;

    const PriceWrapper* wrapper = _prices_connector->get(info.symbol);
    if (wrapper == nullptr)
        return 0.0;

    const BinanceSymbolData::MinNotional& min_notional = info.minNotional;
    const BinanceSymbolData::LotSize& lot_size = info.lotSize;

    // TODO: price average
    // Price price_avg = wrapper->getPriceAverage(min_notional.avgPriceMins * Timer::sMinute);
    Price price_avg = wrapper->get();
    double quantity = std::max(lot_size.minQty, min_notional.minNotional / price_avg) *  1.3;
    if (info.lotSize.stepSize > 0.0)
        quantity = std::round(quantity / info.lotSize.stepSize) * info.lotSize.stepSize;
    return quantity;
}

double BinanceController::roundQuantity(double quantity, const std::string& symbol, double(*fn)(double)) const {
    auto it = _exchange_info.symbols.find(symbol);
    if (it == _exchange_info.symbols.end())
        return 0.0;

    const BinanceSymbolData& info = it->second;

    if (info.lotSize.stepSize > 0.0)
        quantity = fn(quantity / info.lotSize.stepSize) * info.lotSize.stepSize;

    return std::max(quantity, minQuantity(symbol));
}

double BinanceController::fee() const {
    double commission = _commission;

    // Using BNB to pay for fees ( 25% discount )
    if (_balances_connector != nullptr && _balances_connector->get("BNB")->get() > 0.0)
        commission -= commission * 0.25;

    return commission / 100.0;
}

bool BinanceController::checkRateLimits() const {
    for (BinanceRateLimitData& rateLimit : _exchange_info.rateLimits) {
        if (rateLimit.type == "REQUEST_WEIGHT") {
            std::string key = std::to_string(rateLimit.intervalNum) + rateLimit.interval.front();

            int limit = rateLimit.limit;
            int used = BinaCPP::get_usedWeight(util::lowercase(key.c_str()));

            if (used + 20 > limit) {
                Logger::error(util::format("BinanceController::checkRateLimits used %d in %d %s (limit: %d)", used, rateLimit.intervalNum, rateLimit.interval.c_str(), limit));
                return false;
            }

            // Logger::info(util::format("BinanceController::checkRateLimits used %d in %d %s (limit: %d)", used, rateLimit.intervalNum, rateLimit.interval.c_str(), limit));
        }
    }

    return true;
}

BinanceExchangeData BinanceController::_exchange_info;
