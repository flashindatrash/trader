#include "TraderApp.hpp"
#include "proxy/Database.hpp"
#include "proxy/ExchangerProxy.hpp"
#include "proxy/TraderTime.hpp"
#include "proxy/TraderAlgorithm.hpp"
#include "exchanger/base/Symbol.hpp"

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
    if (not DB().init(_config)) return;
    if (not Exchanger().init(_config, symbol)) return;
    if (not SAlgorithm().init(_config, symbol)) return;

    Exchanger().run();
    while (true) {
        sleep_ms(100);
        Time().tick();
    }
}
