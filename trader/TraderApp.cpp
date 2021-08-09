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
#include "wrapper/TradeSymbol.hpp"

#include <thread>
#include <chrono>

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
    BinaCPP_logger::set_debug_level(2);
    BinaCPP_logger::enable_logfile(1);

    // init data
    DB().init();
    STime().init();
    SExchangeInfo().init();
    SAccount().init();
    SPrices().init();
    SKlines().init(symbol);
    SAlgorithm().init(symbol);

    std::thread thread(&TraderApp::thread_websockets, this, symbol);
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    thread.join();
}

void TraderApp::thread_websockets(const TradeSymbol& symbol) {
    BinaCPP_websocket::init();
    SAccount().connect();
    SKlines().connect(symbol);
    BinaCPP_websocket::enter_event_loop();
}
