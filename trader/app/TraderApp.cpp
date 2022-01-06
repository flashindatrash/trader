#include "TraderApp.hpp"
#include "Time.hpp"
#include "Logger.hpp"
#include "database/Database.hpp"
#include "exchanger/Exchanger.hpp"
#include "strategy/Strategy.hpp"

TraderApp* TraderApp::create(const core::Config& config) {
    auto* app = new TraderApp(config);
    return app;
}

TraderApp::TraderApp(const core::Config& config)
    : BaseApp(config, core::Version(1, 4, 0))
{
}

TraderApp::~TraderApp() {
    delete _strategy;
    _strategy = nullptr;
}

int TraderApp::run() {
    Logger::info(util::format("TraderBot %s", _version.toString().c_str()));

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
