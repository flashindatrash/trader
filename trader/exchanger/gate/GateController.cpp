#include "GateController.hpp"
#include "GateWebsocket.hpp"
#include "core/Config.hpp"
#include "core/Logger.hpp"
#include "core/Time.hpp"
#include "util/StringUtil.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/BookWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include <curl/curl.h>
#include <json/json.h>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace {
size_t receive(char* data, size_t size, size_t count, void* target) {
    static_cast<std::string*>(target)->append(data, size * count);
    return size * count;
}

std::string hex(const unsigned char* data, size_t size) {
    std::ostringstream out;
    out << std::hex << std::setfill('0');
    for (size_t i = 0; i < size; ++i)
        out << std::setw(2) << static_cast<unsigned>(data[i]);
    return out.str();
}

std::string sha512(const std::string& value) {
    unsigned char digest[SHA512_DIGEST_LENGTH];
    SHA512(reinterpret_cast<const unsigned char*>(value.data()), value.size(), digest);
    return hex(digest, sizeof(digest));
}

std::string hmac512(const std::string& key, const std::string& value) {
    unsigned char digest[EVP_MAX_MD_SIZE];
    unsigned int size = 0;
    HMAC(EVP_sha512(), key.data(), static_cast<int>(key.size()),
         reinterpret_cast<const unsigned char*>(value.data()), value.size(), digest, &size);
    return hex(digest, size);
}

std::string jsonString(const Json::Value& value) {
    if (value.isString()) return value.asString();
    if (value.isNumeric()) return value.asString();
    return {};
}
}

GateController::~GateController() {
    _running = false;
    if (_thread.joinable())
        _thread.join();
    for (GateWebsocket* websocket : _websockets)
        delete websocket;
}

bool GateController::init(const core::Config& config) {
    _api_key = config.asString("GATE_API_KEY");
    _secret_key = config.asString("GATE_SECRET_KEY");
    if (config.has("GATE_API_URL") && !config.asString("GATE_API_URL").empty())
        _api_url = config.asString("GATE_API_URL");
    while (!_api_url.empty() && _api_url.back() == '/')
        _api_url.pop_back();
    return curl_global_init(CURL_GLOBAL_DEFAULT) == CURLE_OK;
}

void GateController::run() {
    if (_running.exchange(true))
        return;
    _thread = std::thread([this] {
        while (_running) {
            for (GateWebsocket* websocket : _websockets)
                websocket->service(0);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}

void GateController::tick(time_t) {
    for (GateWebsocket* websocket : _websockets)
        websocket->connect();
}

bool GateController::request(const std::string& method, const std::string& path, const std::string& query,
                             const std::string& body, bool authenticated, Json::Value& result) const {
    CURL* curl = curl_easy_init();
    if (curl == nullptr)
        return false;
    const std::string url = _api_url + path + (query.empty() ? "" : "?" + query);
    std::string response;
    curl_slist* headers = nullptr;
    headers = curl_slist_append(headers, "Accept: application/json");
    headers = curl_slist_append(headers, "Content-Type: application/json");
    if (authenticated) {
        if (_api_key.empty() || _secret_key.empty()) {
            print(__func__, "GATE_API_KEY or GATE_SECRET_KEY is empty");
            curl_slist_free_all(headers);
            curl_easy_cleanup(curl);
            return false;
        }
        const std::string timestamp = std::to_string(std::time(nullptr));
        const std::string sign_path = "/api/v4" + path;
        const std::string signature = hmac512(_secret_key, method + "\n" + sign_path + "\n" + query + "\n" + sha512(body) + "\n" + timestamp);
        headers = curl_slist_append(headers, ("KEY: " + _api_key).c_str());
        headers = curl_slist_append(headers, ("Timestamp: " + timestamp).c_str());
        headers = curl_slist_append(headers, ("SIGN: " + signature).c_str());
    }
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, receive);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);
    if (!body.empty())
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
    const CURLcode status = curl_easy_perform(curl);
    long http_status = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_status);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    if (status != CURLE_OK) {
        print(__func__, curl_easy_strerror(status));
        return false;
    }
    Json::CharReaderBuilder builder;
    std::string errors;
    std::istringstream stream(response);
    if (!Json::parseFromStream(builder, stream, &result, &errors)) {
        print(__func__, "invalid JSON: " + errors);
        return false;
    }
    if (http_status < 200 || http_status >= 300) {
        print(__func__, result["label"].asString() + ": " + result["message"].asString());
        return false;
    }
    return true;
}

bool GateController::loadPairs(Storage::Type_pair& container) const {
    Json::Value json;
    if (!request("GET", "/spot/currency_pairs", {}, {}, false, json) || !json.isArray())
        return false;
    _pairs.clear();
    for (const Json::Value& item : json) {
        PairInfo pair;
        pair.id = item["id"].asString();
        pair.base = item["base"].asString();
        pair.quote = item["quote"].asString();
        pair.min_base = Decimal::deserialize(item["min_base_amount"].asString());
        pair.amount_precision = item.get("amount_precision", 8).asUInt();
        pair.tradable = item["trade_status"].asString() == "tradable";
        const std::string symbol = symbolId(pair.id);
        _pairs[symbol] = pair;
        Symbol* wrapper = container.get(symbol);
        if (wrapper->baseAsset().empty() || wrapper->quoteAsset().empty()) {
            wrapper->set(pair.base, pair.quote);
            Symbol::onAdded.emmit(*wrapper);
        }
    }
    return !_pairs.empty();
}

bool GateController::loadPrices(Storage::Type_price& container) const {
    Json::Value json;
    if (!request("GET", "/spot/tickers", {}, {}, false, json) || !json.isArray())
        return false;
    for (const Json::Value& item : json) {
        const std::string symbol = symbolId(item["currency_pair"].asString());
        Ticker ticker;
        ticker.symbol = symbol;
        ticker.time = Time().ms();
        ticker.bestBidPrice = Decimal::deserialize(item["highest_bid"].asString());
        ticker.bestAskPrice = Decimal::deserialize(item["lowest_ask"].asString());
        container.get(symbol)->set(ticker);
        container.get(symbol)->set(Decimal::deserialize(item["last"].asString()));
    }
    return true;
}

bool GateController::loadBalances(Storage::Type_balance& container) const {
    Json::Value json;
    if (!request("GET", "/spot/accounts", {}, {}, true, json) || !json.isArray())
        return false;
    for (const Json::Value& item : json)
        container.get(item["currency"].asString())->set(Decimal::deserialize(item["available"].asString()), Decimal::deserialize(item["locked"].asString()));
    return true;
}

void GateController::connectPrices(Storage::Type_price& container) { loadPrices(container); _prices = &container; }
void GateController::connectBalances(Storage::Type_balance& container) { loadBalances(container); _balances = &container; }
void GateController::connectCharts(Storage::Type_chart& container) { _charts = &container; }

bool GateController::loadCharts(ChartWrapper& container, ChartRequest& chart) const {
    std::string query = "currency_pair=" + pairId(container.id()) + "&interval=" + interval(chart.interval);
    if (chart.time_start) query += "&from=" + std::to_string(chart.time_start / 1000);
    if (chart.time_end) query += "&to=" + std::to_string(chart.time_end / 1000);
    Json::Value json;
    if (!request("GET", "/spot/candlesticks", query, {}, false, json) || !json.isArray())
        return false;
    for (const Json::Value& item : json) {
        if (!item.isArray() || item.size() < 6) continue;
        Candlestick candle;
        candle.symbol = container.id();
        candle.time_open = std::stoll(jsonString(item[0])) * 1000;
        candle.time_close = candle.time_open;
        candle.price_close = Decimal::deserialize(jsonString(item[2]));
        candle.price_high = Decimal::deserialize(jsonString(item[3]));
        candle.price_low = Decimal::deserialize(jsonString(item[4]));
        candle.price_open = Decimal::deserialize(jsonString(item[5]));
        candle.closed = true;
        container.add(candle);
    }
    return true;
}

void GateController::listenTicker(PriceWrapper& container) {
    const std::string payload = "[\"" + pairId(container.id()) + "\"]";
    addWebsocket("spot.tickers", payload, [this](const Json::Value& json) { onTicker(json); });
}

void GateController::listenCharts(ChartWrapper& container, ChartInterval chart_interval) {
    const std::string payload = "[\"" + interval(chart_interval) + "\",\"" + pairId(container.id()) + "\"]";
    addWebsocket("spot.candlesticks", payload, [this](const Json::Value& json) { onCandle(json); });
}

void GateController::addWebsocket(const std::string& channel, const std::string& payload,
                                  std::function<void(const Json::Value&)> callback) {
    if (std::any_of(_websockets.begin(), _websockets.end(), [&](GateWebsocket* item) { return item->matches(channel, payload); }))
        return;
    auto* websocket = new GateWebsocket(channel, payload, std::move(callback));
    _websockets.push_back(websocket);
    websocket->connect();
}

void GateController::onTicker(const Json::Value& json) {
    if (_prices == nullptr) return;
    const Json::Value& item = json["result"];
    Ticker ticker;
    ticker.symbol = symbolId(item["currency_pair"].asString());
    ticker.time = json["time_ms"].asInt64();
    ticker.bestBidPrice = Decimal::deserialize(item["highest_bid"].asString());
    ticker.bestAskPrice = Decimal::deserialize(item["lowest_ask"].asString());
    PriceWrapper* price = _prices->get(ticker.symbol);
    price->set(ticker);
    price->set(Decimal::deserialize(item["last"].asString()));
}

void GateController::onCandle(const Json::Value& json) {
    if (_charts == nullptr) return;
    const Json::Value& item = json["result"];
    Candlestick candle;
    candle.symbol = symbolId(item["n"].asString().substr(item["n"].asString().find('_') + 1));
    candle.time_open = std::stoll(jsonString(item["t"])) * 1000;
    candle.time_close = candle.time_open;
    candle.price_open = Decimal::deserialize(item["o"].asString());
    candle.price_high = Decimal::deserialize(item["h"].asString());
    candle.price_low = Decimal::deserialize(item["l"].asString());
    candle.price_close = Decimal::deserialize(item["c"].asString());
    candle.closed = item.get("w", false).asBool();
    if (_prices != nullptr) _prices->get(candle.symbol)->set(candle.price_close);
    _charts->get(candle.symbol)->add(candle);
}

const OrderWrapper* GateController::createOrder(BookWrapper& container, OrderRequest& order) {
    const auto found = _pairs.find(order.symbol);
    if (found == _pairs.end() || !found->second.tradable || order.side == OrderSide::Invalid)
        return nullptr;
    order.quantity = roundQuantity(order.quantity, order.symbol);
    if (order.mask(OrderRequest::CheckBalance) && OrderUtil::usedAsset(order.side, order.symbol).balance() < order.required())
        return nullptr;
    OrderDetail detail;
    detail.symbol = order.symbol;
    detail.side = order.side;
    detail.base_quantity = order.quantity;
    if (order.mask(OrderRequest::TestMode)) {
        detail.id = std::to_string(Time().ms());
        detail.quote_quantity = order.quantity * order.symbol.price(order.side);
    } else {
        Json::Value body_json;
        body_json["currency_pair"] = found->second.id;
        body_json["type"] = "market";
        body_json["account"] = "spot";
        body_json["side"] = order.side == OrderSide::Buy ? "buy" : "sell";
        body_json["time_in_force"] = "ioc";
        const Decimal amount = order.side == OrderSide::Buy ? order.quantity * order.symbol.price(order.side) : order.quantity;
        body_json["amount"] = static_cast<const std::string&>(amount);
        Json::StreamWriterBuilder writer;
        writer["indentation"] = "";
        Json::Value response;
        if (!request("POST", "/spot/orders", {}, Json::writeString(writer, body_json), true, response))
            return nullptr;
        detail.id = response["id"].asString();
        detail.base_quantity = Decimal::deserialize(response["filled_amount"].asString());
        detail.quote_quantity = Decimal::deserialize(response["filled_total"].asString());
    }
    const OrderWrapper* wrapper = container.add(detail);
    if (wrapper != nullptr) wrapper->operate();
    return wrapper;
}

Decimal GateController::roundQuantity(Decimal quantity, const std::string& symbol) const {
    const auto found = _pairs.find(symbol);
    if (found == _pairs.end()) return {};
    const unsigned precision = std::min(found->second.amount_precision, 8u);
    Decimal::IntType step = 1;
    for (unsigned i = precision; i < 8; ++i) step *= 10;
    if (step > 1) quantity = (static_cast<Decimal::IntType>(quantity) / step) * step;
    return std::max(quantity, found->second.min_base);
}

double GateController::fee() const { return _commission; }
std::string GateController::pairId(const std::string& symbol) const { auto it = _pairs.find(symbol); return it == _pairs.end() ? symbol : it->second.id; }
std::string GateController::symbolId(const std::string& pair) { std::string result = pair; result.erase(std::remove(result.begin(), result.end(), '_'), result.end()); return result; }
std::string GateController::interval(ChartInterval value) { switch (value) { case m5: return "5m"; case m15: return "15m"; case h1: return "1h"; case d1: return "1d"; } return {}; }
void GateController::print(const std::string& context, const std::string& message) const { Logger::info(util::format("GateController::%s: %s", context.c_str(), message.c_str())); }
