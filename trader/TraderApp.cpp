#include "TraderApp.hpp"
#include "binacpp.h"
#include "binacpp_websocket.h"
#include "binacpp_logger.h"
#include "Config.hpp"
#include "proxy/Database.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/BinanceAccount.hpp"
#include "proxy/BinanceAlgorithm.hpp"
#include "proxy/BinanceOrders.hpp"
#include "proxy/BinancePrices.hpp"
#include "proxy/BinanceExchangeInfo.hpp"
#include "proxy/BinanceKlines.hpp"

TraderApp::TraderApp() {
}

TraderApp* TraderApp::create() {
    TraderApp* app = new TraderApp();
    return app;
}

void TraderApp::run(const TradeSymbol& symbol) {
    // init binance api
    static string api_key       = BINANCE_API_KEY;
    static string secret_key    = BINANCE_SECRET_KEY;
    BinaCPP::init(api_key, secret_key);
    // init binance logger
    BinaCPP_logger::set_debug_level(0);

    // init data
    DB().init();
    STime().init();
    SExchangeInfo().init();
    SAccount().init();
    SPrices().init();
    SKlines().init(symbol);
    SAlgorithm().init(symbol);

    // connect websocket
    BinaCPP_websocket::init();
    SAccount().connect();
    //SPrices().connect(symbol);
    SKlines().connect(symbol);
    BinaCPP_websocket::enter_event_loop();
}
