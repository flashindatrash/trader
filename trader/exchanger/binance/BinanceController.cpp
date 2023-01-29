#include "BinanceController.hpp"
#include "BinanceWebsocket.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/BookWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/wrapper/StakingWrapper.hpp"
#include "response/BinanceEnums.hpp"
#include "response/BinanceErrorData.hpp"
#include "response/BinanceBalanceData.hpp"
#include "response/BinanceOrderData.hpp"
#include "response/BinancePriceStatisticsData.hpp"
#include "response/BinanceKlineData.hpp"
#include "response/BinanceTickerData.hpp"
#include "response/BinanceFlexibleBalanceData.hpp"
#include "response/BinanceSpotAccountData.hpp"
#include "response/BinancePriceData.hpp"
#include "response/BinanceStakingProductData.hpp"
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"
#include "binacpp_utils.h"
#include "core/Config.hpp"
#include "core/Logger.hpp"
#include "core/Time.hpp"
#include <cmath>
#include <algorithm>

static const char* CONFIG_API_KEY = "BINANCE_API_KEY";
static const char* CONFIG_SECRET_KEY = "BINANCE_SECRET_KEY";
static const char* CONFIG_RECV_WINDOW = "BINANCE_RECV_WINDOW";

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

    // check time sync
    if (not checkServerTime())
        return false;

    // init user data stream
    if (not initUserListenKey())
        return false;

    return true;
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

    if (checkError(json, __func__))
        return false;

    BinanceExchangeData data(json);
    if (data.symbols.empty()) {
        print(__func__, util::format("invalid json %s", json.toStyledString().c_str()));
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

    if (checkError(json, __func__))
        return false;

    if (not json.isArray()) {
        print(__func__, util::format("invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (auto & it : json) {
        BinancePriceData data(it);
        container.get(data.symbol)->set(data.price);
    }

    return true;
}

bool BinanceController::loadStakings(Storage::Type_staking& container) const {
    for (StakingProduct product : { Locked, DeFiLocked, DeFiFlexible }) {
        long current = 1; long size = 100;
        while (true) {
            if (not checkRateLimits())
                return false;

            Json::Value json;
            BinaCPP::get_stakingProjects(binance::serialize(product).c_str(), "", current, size, _config_recv_window, json);

            if (checkError(json, __func__))
                return false;

            if (not json.isArray()) {
                print(__func__, util::format("invalid json %s", json.toStyledString().c_str()));
                return false;
            }

            for (auto &it: json) {
                BinanceStakingProductData data(it);
                container.get(data.projectId)->set(product, data.detail, data.quota);
            }

            if (json.size() < size)
                break;
            else ++current;
        }
    }

    return true;
}

bool BinanceController::loadPrice(PriceWrapper& container) const {
    if (not checkRateLimits())
        return false;

    Json::Value json;
    BinaCPP::get_prices(container.id().c_str(), json);

    if (checkError(json, __func__))
        return false;

    if (not json.isArray()) {
        print(__func__, util::format("invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (auto & it : json) {
        BinancePriceData data(it);
        if (container.id() != data.symbol) {
            print(__func__, util::format("invalid container %s for symbol %s", container.id().c_str(), data.symbol.c_str()));
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

    if (checkError(json, __func__))
        return false;

    BinanceSpotAccountData account(json);
    if (not account.canTrade) {
        print(__func__, "can't trade on spot account");
        return false;
    }

    _commission = account.takerCommission / 100.0;

    for (const BinanceBalanceData& balance : account.balances)
        container.get(balance.asset)->set(balance.free, balance.locked);

    return true;
}

bool BinanceController::redeemSavings(const std::string& asset, Decimal quantity) const {
    if (not checkRateLimits())
        return false;

    if (Asset(asset).ld().balance() < quantity)
        return false;

    Json::Value json;
    BinaCPP::get_flexibleProducts(asset.c_str(), _config_recv_window, json);

    if (checkError(json, __func__))
        return false;

    if (not json.isArray()) {
        print(__func__, util::format("invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (const auto & product : json) {
        BinanceFlexibleBalanceData data(product);

        if (not data.canRedeem || data.free < quantity || data.redeemingAmount > 0.0)
            continue;

        Json::Value json_redeem;
        BinaCPP::redeem_flexibleProduct(data.productId.c_str(), quantity.c_str(), "FAST", _config_recv_window, json_redeem);

        if (checkError(json_redeem, __func__))
            return false;

        return true;
    }

    return false;
}

bool BinanceController::loadStats(CandlestickWrapper& container) const {
    if (not checkRateLimits())
        return false;

    Json::Value json;
    BinaCPP::get_24hr(container.id().c_str(), json);

    if (checkError(json, __func__))
        return false;

    container.set(BinancePriceStatisticsData(json));
    return true;
}

bool BinanceController::loadCharts(ChartWrapper& container, ChartRequest& request) const {
    if (not checkRateLimits())
        return false;

    const std::string& interval_converted = binance::serialize(request.interval);
    if (interval_converted.empty()) {
        print(__func__, "unknown chart interval");
        return false;
    }

    Json::Value json;
    BinaCPP::get_klines(container.id().c_str(), interval_converted.c_str(), 0, request.time_start, request.time_end, json);

    if (checkError(json, __func__))
        return false;

    if (not json.isArray()) {
        print(__func__, util::format("invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (auto& item : json) {
        BinanceKlineData data(item);
        data.symbol = container.id();

        if (container.add(data) == nullptr)
            print(__func__, util::format("invalid kline %s", item.toStyledString().c_str()));
    }

    return true;
}

bool BinanceController::loadOrders(BookWrapper& container) const {
    if (not checkRateLimits())
        return false;

    Json::Value json;
    BinaCPP::get_allOrders(container.id().c_str(), 0, 0, _config_recv_window, json);

    if (checkError(json, __func__))
        return false;

    if (not json.isArray()) {
        print(__func__, util::format("invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    for (const auto& item : json) {
        if (not container.add(BinanceOrderData(item, false)))
            print(__func__, util::format("invalid order: %s", item.toStyledString().c_str()));
    }

    return true;
}

void BinanceController::connectPrices(Storage::Type_price& container) {
    loadPrices(container);
    _prices_connector = &container;
}

void BinanceController::connectBalances(Storage::Type_balance& container) {
    loadBalances(container);
    // savings already loaded in api/v3/account
    // loadSavings(container);
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
    addWebsocket(websocket);
}

void BinanceController::listenTicker(PriceWrapper& container) {
    const std::string& path = util::lowercase(container.id().c_str()) + "@bookTicker";
    if (findWebsocket(path) != _websockets.end())
        return;

    BinanceWebsocket* websocket = BinanceWebsocket::create();
    websocket->setPath(path);
    websocket->setCallback(std::bind(&BinanceController::onTickerDataStream, this, std::placeholders::_1));
    addWebsocket(websocket);
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

    if (checkError(json, __func__))
        return false;

    if (not json["listenKey"] || not json["listenKey"].isString()) {
        print(__func__, util::format("invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    BinanceWebsocket* websocket = BinanceWebsocket::create();
    websocket->setPath(json["listenKey"].asString());
    websocket->setType(BinanceWebsocket::UserStream);
    websocket->setCallback(std::bind(&BinanceController::onUserDataStream, this, std::placeholders::_1));
    addWebsocket(websocket);
    return true;
}

bool BinanceController::keepUserDataStream() {
    if (not checkRateLimits())
        return false;

    auto it = std::find_if(_websockets.begin(), _websockets.end(), [](BinanceWebsocket* i) { return i->type() == BinanceWebsocket::UserStream; });
    if (it == _websockets.end())
        return false;

    BinanceWebsocket* websocket = *it;
    if (not websocket->connect())
        return false;

    BinaCPP::keep_userDataStream(websocket->path().c_str());
    return true;
}

bool BinanceController::addWebsocket(BinanceWebsocket* websocket) {
    _websockets.push_back(websocket);
    return websocket->connect();
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
                Logger::info( util::format("order rejected %s", json["r"].asString().c_str()));
        }
    } else if (action == "outboundAccountPosition") {
        for (const auto &i : json["B"]) {
            BinanceBalanceData data(i, "a", "f", "l");
            Logger::info(util::format("balance %s: %s", data.asset.c_str(), data.free.c_str()));
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

bool BinanceController::checkWalletRequest(WalletRequest& request, const std::string& asset, const Decimal& quantity) const {
    if (request.mask(OrderRequest::CheckBalance) && Asset(asset).balance() < quantity) {
        // policy do not allow redeeming
        if (not request.mask(OrderRequest::RedeemSavings))
            return false;

        // try to redeem from savings
        Decimal redeem_quantity = quantity - Asset(asset).balance();
        if (not redeemSavings(asset, redeem_quantity)) {
            print(__func__, util::format("failed to redeem %s %s", redeem_quantity.c_str(), asset.c_str()));
            return false;
        }

        // check after redeeming, in can be in progress
        int tries = 0;
        while(Asset(asset).balance() < quantity) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            if (++tries > 5)
                return false;
        }
    }

    return true;
}

const OrderWrapper* BinanceController::createOrder(BookWrapper& container, OrderRequest& request) {
    if (not checkRateLimits())
        return nullptr;

    if (request.side == OrderSide::Invalid)
        return nullptr;

    std::string type = binance::serialize(request.type);

    auto it = _exchange_info.symbols.find(request.symbol);
    if (it == _exchange_info.symbols.end()) {
        print(__func__, "unknown symbol");
        return nullptr;
    }

    const BinanceSymbolData& info = it->second;
    if (not info.hasOrderType(type)) {
        print(__func__, "symbol doesn't supported for this type");
        return nullptr;
    }

    if (not info.isSpotTradingAllowed) {
        print(__func__, "spot trading is not allowed");
        return nullptr;
    }

    // round quantity to step size and min lot
    request.quantity = roundQuantity(request.quantity, request.symbol);

    // check is enough to create order
    const Asset& asset = OrderUtil::usedAsset(request.side, request.symbol);
    if (not checkWalletRequest(request, asset, request.required()))
        return nullptr;

    Json::Value json;
    BinaCPP::send_order(request.symbol.c_str(), binance::serialize(request.side).c_str(), type.c_str(), "GTC", request.quantity.c_str(), {}, "", 0, 0, request.mask(OrderRequest::TestMode), _config_recv_window, json);

    BinanceOrderData data;
    if (json.empty() && request.mask(OrderRequest::TestMode)) {
        // empty response & test mode
        data.status = "test mode";
        data.id = to_string(Time().ms());
        data.symbol = request.symbol;
        data.side = request.side;
        data.base_quantity = request.quantity;
        data.quote_quantity = request.quantity * request.symbol.price(request.side);
    } else if (checkError(json, __func__)) {
        // error response
        return nullptr;
    } else {
        // normal response
        data = BinanceOrderData(json, false);
    }

    if (data.status.empty()) {
        print(__func__, "empty response");
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

bool BinanceController::stake(StakingWrapper& container, StakingRequest& request) {
    if (not checkRateLimits())
        return false;

    // check is enough to stake
    if (not checkWalletRequest(request, container.asset(), request.amount))
        return false;

    if (request.mask(StakingRequest::TestMode));
    else {
        Json::Value json;
        BinaCPP::stake(binance::serialize(container.product()).c_str(), container.id().c_str(), request.amount.c_str(), _config_recv_window, json);

        if (checkError(json, __func__))
            return false;

        if (json["success"] != true)
            return false;
    }

    // update personal quota
    container.updateQuota(container.left() - request.amount);
    return true;
}

Decimal BinanceController::minQuantity(const std::string& symbol) const {
    auto it = _exchange_info.symbols.find(symbol);
    if (it == _exchange_info.symbols.end())
        return {};

    const BinanceSymbolData& info = it->second;
    if (_prices_connector == nullptr)
        return {};

    const PriceWrapper* wrapper = _prices_connector->get(info.symbol);
    if (wrapper == nullptr)
        return {};

    const BinanceSymbolData::MinNotional& min_notional = info.minNotional;
    const BinanceSymbolData::LotSize& lot_size = info.lotSize;

    // TODO: price average
    // Price price_avg = wrapper->getPriceAverage(min_notional.avgPriceMins * Timer::sMinute);
    Price price_avg = wrapper->get();
    Decimal quantity = std::max(lot_size.minQty, min_notional.minNotional / price_avg) *  1.3;
    if (info.lotSize.stepSize > Decimal::Zero)
        quantity = ((Decimal::IntType)quantity / (Decimal::IntType)info.lotSize.stepSize) * (Decimal::IntType)info.lotSize.stepSize;
    return quantity;
}

bool BinanceController::updateStaking(StakingWrapper& container) const {
    if (not checkRateLimits())
        return false;

    Json::Value json;
    BinaCPP::get_stakingLeftQuota(binance::serialize(container.product()).c_str(), container.id().c_str(), _config_recv_window, json);

    if (checkError(json, __func__))
        return false;

    Decimal quota = Decimal::deserialize(json["leftPersonalQuota"].asString());
    container.updateQuota(quota);
    return true;
}

Decimal BinanceController::roundQuantity(Decimal quantity, const std::string& symbol) const {
    auto it = _exchange_info.symbols.find(symbol);
    if (it == _exchange_info.symbols.end())
        return {};

    const BinanceSymbolData& info = it->second;

    if (info.lotSize.stepSize > Decimal::Zero)
        quantity = ((Decimal::IntType)quantity / (Decimal::IntType)info.lotSize.stepSize) * (Decimal::IntType)info.lotSize.stepSize;

    return std::max(quantity, minQuantity(symbol));
}

double BinanceController::fee() const {
    double commission = _commission;

    // Using BNB to pay for fees ( 25% discount )
    if (_balances_connector != nullptr && _balances_connector->get("BNB")->get() > Decimal::Zero)
        commission -= commission * 0.25;

    return commission / 100.0;
}

bool BinanceController::checkError(const Json::Value& json, const std::string& context) const {
    BinanceErrorData error(json);
    if (not error.has())
        return false;

    print(context, util::format("%s [%d]", error.msg.c_str(), error.code));

    if (error.code == BinanceErrorData::INVALID_TIMESTAMP)
        checkServerTime();

    if (error.code == BinanceErrorData::TOO_MANY_REQUESTS)
        Logger::error(error.msg);

    return true;
}

void BinanceController::print(const std::string& context, const std::string& msg) const {
    Logger::info(util::format("BinanceController::%s: %s", context.c_str(), msg.c_str()));
}

bool BinanceController::checkServerTime() const {
    if (not checkRateLimits())
        return false;

    Json::Value json;
    BinaCPP::get_serverTime(json);

    if (checkError(json, __func__))
        return false;

    if (not json.isObject() || not json["serverTime"] || not json["serverTime"].isInt64()) {
        print(__func__, util::format("invalid json %s", json.toStyledString().c_str()));
        return false;
    }

    unsigned long server_time = json["serverTime"].asInt64();
    unsigned long local_time = get_current_ms_epoch();
    long shift_time = (long)(server_time - local_time);

    if (local_time >= (server_time + 1000) || shift_time > _config_recv_window)
        print(__func__,util::format("time out of sync (shift %ld)", shift_time));

    BinaCPP_time::shift = shift_time;
    return true;
}

bool BinanceController::checkRateLimits() const {
    for (BinanceRateLimitData& rateLimit : _exchange_info.rateLimits) {
        if (rateLimit.type == "REQUEST_WEIGHT") {
            std::string key = std::to_string(rateLimit.intervalNum) + rateLimit.interval.front();

            int limit = rateLimit.limit;
            int used = BinaCPP::get_usedWeight(util::lowercase(key.c_str()));

            if (used + 20 > limit) {
                Logger::error(util::format("%s: used %d in %d %s (limit: %d)", __func__, used, rateLimit.intervalNum, rateLimit.interval.c_str(), limit));
                return false;
            }

        }
    }

    return true;
}

BinanceExchangeData BinanceController::_exchange_info;
double BinanceController::_commission;