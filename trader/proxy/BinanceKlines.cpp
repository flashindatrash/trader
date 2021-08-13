#include "binacpp.h"
#include "binacpp_websocket.h"
#include "Logger.hpp"
#include "proxy/BinancePrices.hpp"
#include "proxy/BinanceKlines.hpp"
#include "exchanger/wrapper/TradeSymbol.hpp"
#include "exchanger/wrapper/PriceSymbol.hpp"
#include "exchanger/wrapper/KlineHistory.hpp"
#include "exchanger/wrapper/Candlestick.hpp"
#include "exchanger/binance/response/BinanceKlineData.hpp"
#include "exchanger/binance/response/BinanceErrorData.hpp"
#include "util/StringUtil.hpp"

static const std::string& sInterval = "15m";

BinanceKlines::~BinanceKlines() {
    for (auto& pair : _histories)
        SAFE_DELETE(pair.second);
    _histories.clear();
}

void BinanceKlines::init(const TradeSymbol& symbol) {
    Json::Value result;
    BinaCPP::get_klines(symbol.c_str(), sInterval.c_str(), 10, 0, 0, result);

    BinanceErrorData error(result);
    if (error.has()) {
        logic_error(error.msg.c_str());
        return;
    }

    if (not result.isArray()) {
        trace("%s\n", result.toStyledString().c_str());
        logic_error("invalid klines");
        return;
    }

    for (Json::ArrayIndex i = 0; i < result.size(); ++i) {
        BinanceKlineData data(result[i]);
        if (data.isEmpty())
            continue;

        data.symbol = symbol;
        add(data);
    }

    auto klines = _histories.at(symbol)->klines(); int j = 0;
    for (auto it = klines.rbegin(); it < klines.rend() - 1; ++it) {
        const BinanceKlineData& current_data = *it;
        const BinanceKlineData& previous_data = *(it + 1);

        Candlestick current(current_data.priceOpen, current_data.priceHigh, current_data.priceLow, current_data.priceClose);
        Candlestick previous(previous_data.priceOpen, previous_data.priceHigh, previous_data.priceLow, previous_data.priceClose);

        trace("--%d--\n", j++);
        if (Candlestick::isHammer(current)) { trace("> isHammer\n"); }
        if (Candlestick::isInvertedHammer(current)) { trace("> isInvertedHammer\n"); }
        if (Candlestick::isHangingMan(previous, current)) { trace("> isHangingMan\n"); }
        if (Candlestick::isShootingStar(previous, current)) { trace("> isShootingStar\n"); }
        if (Candlestick::isBullishEngulfing(previous, current)) { trace("> isBullishEngulfing\n"); }
        if (Candlestick::isBearishEngulfing(previous, current)) { trace("> isBearishEngulfing\n"); }
        if (Candlestick::isBullishHarami(previous, current)) { trace("> isBullishHarami\n"); }
        if (Candlestick::isBearishHarami(previous, current)) { trace("> isBearishHarami\n"); }
        if (Candlestick::isBullishKicker(previous, current)) { trace("> isBullishKicker\n"); }
        if (Candlestick::isBearishKicker(previous, current)) { trace("> isBearishKicker\n"); }
    }

    const std::string& path = "/ws/" + util::lowercase(symbol.c_str()) + "@kline_" + sInterval;
    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceKlines::handle, this, std::placeholders::_1), path.c_str());
}

int BinanceKlines::handle(Json::Value& json) {
    BinanceKlineData data(json);
    if (data.isEmpty())
        return 0;

    // update price
    if (PriceSymbol* wrapper = SPrices().getMutablePrice(data.symbol))
        wrapper->add(data.priceClose);

    // add & invoke listeners
    add(data);
    invoke(data);
    return 0;
}

void BinanceKlines::add(const BinanceKlineData& data) {
    if (data.symbol.empty())
        return;

    KlineHistory* history = nullptr;
    auto it = _histories.find(data.symbol);
    if (it == _histories.end()) {
        history = KlineHistory::create();
        _histories[data.symbol] = history;
    } else
        history = it->second;
    history->add(data);
}

KlineHistory* BinanceKlines::get(const TradeSymbol& symbol) const {
    auto it = _histories.find(symbol);
    if (it == _histories.end())
        return nullptr;
    return it->second;
}
