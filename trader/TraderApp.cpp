#include "TraderApp.hpp"
#include <thread>
#include <chrono>
#include "Config.hpp"
#include "proxy/Database.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/BinanceAlgorithm.hpp"
#include "proxy/BinanceOrders.hpp"
#include "proxy/BinanceKlines.hpp"
#include "exchanger/wrapper/Symbol.hpp"

core::Version TraderApp::sVersion = core::Version(1, 0);

TraderApp::TraderApp(core::Config config)
    : core::App(config)
{
}

TraderApp* TraderApp::create(core::Config config) {
    TraderApp* app = new TraderApp(config);
    return app;
}

void TraderApp::run(const Symbol& symbol) {
    DB().init(_config);
    Exchanger().init(_config);
    STime().init();
    SKlines().init(symbol);
    SAlgorithm().init(symbol);

    Exchanger().run();
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        STime().tick();
    }
}
