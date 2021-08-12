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

core::Version TraderApp::sVersion = core::Version(1, 0);

TraderApp::TraderApp(core::Config config)
    : core::App(config)
{
}

TraderApp* TraderApp::create(core::Config config) {
    TraderApp* app = new TraderApp(config);
    return app;
}

void TraderApp::run(const TradeSymbol& symbol) {
    // init binance logger
    BinaCPP_logger::set_debug_level(2);
    BinaCPP_logger::enable_logfile(1);

    // init binance api
    static string api_key       = _config.getAsString("BINANCE_API_KEY");
    static string secret_key    = _config.getAsString("BINANCE_SECRET_KEY");

    BinaCPP::init(api_key, secret_key);
    BinaCPP_websocket::init();

    // init data
    DB().init(_config);
    STime().init();
    SExchangeInfo().init();
    SAccount().init();
    SPrices().init();
    SKlines().init(symbol);
    SAlgorithm().init(symbol);

    std::thread thread(&BinaCPP_websocket::enter_event_loop);
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        STime().tick();
    }

    thread.join();
}
