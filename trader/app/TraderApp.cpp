#include "TraderApp.hpp"
#include "Time.hpp"
#include "Logger.hpp"
#include "database/Database.hpp"
#include "exchanger/Exchanger.hpp"
#include "strategy_pair/Strategy.hpp"
#include "strategy_listing/Strategy.hpp"

TraderApp* TraderApp::create(const std::string& type, const core::Config& config) {
    auto* app = new TraderApp(config);
    if (app->init(type)) {
        return app;
    }

    delete app;
    return nullptr;
}

TraderApp::TraderApp(const core::Config& config)
    : core::App(config, core::Version(1, 4, 0))
{
}

TraderApp::~TraderApp() {
    delete _strategy;
    _strategy = nullptr;
}

bool TraderApp::init(const std::string& type) {
    if (type == "pair")
        _strategy = new pair::Strategy();
    else if (type == "listing")
        _strategy = new listing::Strategy();

    return _strategy != nullptr;
}

int TraderApp::run() {
    Logger::info(util::format("TraderBot %s", _version.toString().c_str()));

    // init database
    if (not DB().init(_config))
        return EXIT_FAILURE;

    // init exchanger
    if (not Exchanger().init(_config))
        return EXIT_FAILURE;

    // init strategy
    if (not _strategy->init(_config))
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
