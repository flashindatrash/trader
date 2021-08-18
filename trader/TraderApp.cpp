#include "TraderApp.hpp"
#include "proxy/Time.hpp"
#include "proxy/Database.hpp"
#include "proxy/Exchanger.hpp"
#include "strategy/Strategy.hpp"

core::Version TraderApp::sVersion = core::Version(1, 1);

TraderApp::TraderApp(const core::Config& config)
    : core::App(config)
{
}

TraderApp::~TraderApp() {
    SAFE_DELETE(_strategy);
}

TraderApp* TraderApp::create(const core::Config& config) {
    TraderApp* app = new TraderApp(config);
    return app;
}

int TraderApp::run() {
    if (not DB().init(_config)) return 0;
    if (not Exchanger().init(_config)) return 0;

    _strategy = Strategy::create();
    if (not _strategy->init(_config)) return 0;

    Exchanger().run();
    while (true) {
        sleep_ms(100);
        Time().tick();
    }

    return 1;
}
