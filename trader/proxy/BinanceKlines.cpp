#include "binacpp.h"
#include "binacpp_websocket.h"
#include "Logger.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "proxy/BinanceKlines.hpp"
#include "exchanger/wrapper/Symbol.hpp"
#include "exchanger/wrapper/PriceContainer.hpp"
#include "exchanger/wrapper/CandlestickContainer.hpp"
#include "exchanger/binance/response/BinanceKlineData.hpp"
#include "exchanger/binance/response/BinanceErrorData.hpp"
#include "util/StringUtil.hpp"

static const std::string& sInterval = "15m";

BinanceKlines::~BinanceKlines() {
    for (auto& pair : _histories)
        SAFE_DELETE(pair.second);
    _histories.clear();
}

void BinanceKlines::init(const Symbol& symbol) {
    Json::Value result;
    BinaCPP::get_klines(symbol.c_str(), sInterval.c_str(), 40, 0, 0, result);

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

    const std::string& path = "/ws/" + util::lowercase(symbol.c_str()) + "@kline_" + sInterval;
    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceKlines::handle, this, std::placeholders::_1), path.c_str());
}

int BinanceKlines::handle(Json::Value& json) {
    BinanceKlineData data(json);
    if (data.isEmpty())
        return 0;

    // update price
    if (PriceContainer* wrapper = Exchanger().price(data.symbol))
        wrapper->add(data.priceClose);

    // add & invoke listeners
    add(data);
    return 0;
}

void BinanceKlines::add(const BinanceKlineData& data) {
    if (data.symbol.empty())
        return;

    CandlestickContainer* history = nullptr;
    auto it = _histories.find(data.symbol);
    if (it == _histories.end()) {
        history = CandlestickContainer::create();
        _histories[data.symbol] = history;
    } else
        history = it->second;
    history->add(data);
}

CandlestickContainer* BinanceKlines::get(const Symbol& symbol) const {
    auto it = _histories.find(symbol);
    if (it == _histories.end())
        return nullptr;
    return it->second;
}
