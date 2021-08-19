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
    // init database
    if (not DB().init(_config))
        return EXIT_FAILURE;

    // init exchanger
    if (not Exchanger().init(_config))
        return EXIT_FAILURE;

    // create strategy
    _strategy = Strategy::create();
    if (not _strategy->init(_config)) {
        SAFE_DELETE(_strategy);
        return 0;
    }

    // run exchanger thread
    Exchanger().run();

    // run main thread
    while (true) {
        sleep_ms(100);
        Time().tick();
    }

    return 1;
}
