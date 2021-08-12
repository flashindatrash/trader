#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"
#include "Config.hpp"
#include "controller/binance/BinanceController.hpp"

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
