#include "BinanceController.hpp"
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"
#include "Config.hpp"
#include "Logger.hpp"
#include "response/BinanceErrorData.hpp"

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
}

void BinanceController::run() {
    _thread = std::thread(&BinaCPP_websocket::enter_event_loop);
}

std::vector<std::pair<Symbol, Price>> BinanceController::getAllPrices() {
    std::vector<std::pair<Symbol, Price>> vec;

    Json::Value result;
    BinaCPP::get_allPrices(result);

    BinanceErrorData error(result);
    if (error.has()) {
        logic_error(error.msg.c_str());
        return vec;
    }

    if (not result.isArray()) {
        trace("%s\n", result.toStyledString().c_str());
        logic_error("invalid prices");
        return vec;
    }

    for (uint i = 0; i < result.size(); ++i) {
        const Json::Value& data = result[i];

        Symbol symbol = data["symbol"].asString();
        Price price = atof(data["price"].asString().c_str());
        vec.push_back(std::make_pair(symbol, price));
    }

    return vec;
}
