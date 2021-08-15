#include "TraderApp.hpp"
#include "proxy/Database.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "proxy/BinanceTime.hpp"
#include "proxy/BinanceAlgorithm.hpp"
#include "proxy/BinanceOrders.hpp"
#include "exchanger/wrapper/Symbol.hpp"

core::Version TraderApp::sVersion = core::Version(1, 1);

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
    SAlgorithm().init(symbol);

    Exchanger().connect(symbol);
    Exchanger().run();
    while (true) {
        sleep_ms(100);
        STime().tick();
    }
}
