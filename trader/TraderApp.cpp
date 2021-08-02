#include "TraderApp.hpp"

#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"

#include "BinanceConfig.hpp"
#include "proxy/BinanceAccount.hpp"
#include "proxy/BinanceBook.hpp"
#include "proxy/BinanceAlgorithm.hpp"

TraderApp::TraderApp() {
}

TraderApp* TraderApp::create() {
    TraderApp* app = new TraderApp();
    app->run();
    return app;
}

void TraderApp::init() {
    BinanceSymbol symbol(BinanceAsset("BTC"), BinanceAsset("USDT"));

    // init binance api
    static string api_key       = BINANCE_API_KEY;
    static string secret_key    = BINANCE_SECRET_KEY;
    BinaCPP::init( api_key , secret_key );
    // init binance logger
    BinaCPP_logger::set_debug_level(0);

    // init binance websocket
    BinaCPP_websocket::init();
    SBinanceAccount().connect();
    SBinanceBook().connect(symbol);
    SBinanceAlgorithm().connect(symbol);
    BinaCPP_websocket::enter_event_loop();
}
