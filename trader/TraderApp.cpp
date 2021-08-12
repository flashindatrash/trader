#include <thread>
#include <chrono>
#include "TraderApp.hpp"
#include "Config.hpp"
#include "proxy/Database.hpp"
#include "proxy/StockProxy.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/BinanceAccount.hpp"
#include "proxy/BinanceAlgorithm.hpp"
#include "proxy/BinanceOrders.hpp"
#include "proxy/BinancePrices.hpp"
#include "proxy/BinanceExchangeInfo.hpp"
#include "proxy/BinanceKlines.hpp"
#include "wrapper/TradeSymbol.hpp"

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
    DB().init(_config);
    Stock().init(_config);
    STime().init();
    SExchangeInfo().init();
    SAccount().init();
    SPrices().init();
    SKlines().init(symbol);
    SAlgorithm().init(symbol);

    Stock().run();
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        STime().tick();
    }
}
