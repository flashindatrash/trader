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
    _strategy = Strategy::create(_config);
    if (_strategy == nullptr)
        return EXIT_FAILURE;

    // run exchanger thread
    Exchanger().run();

    // run main thread
    while (_strategy->isRunning()) {
        sleep_ms(100);
        Time().tick();
    }

    // stop exchanger thread
    Exchanger().stop();

    return EXIT_SUCCESS;
}
