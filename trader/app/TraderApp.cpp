#include "TraderApp.hpp"

#include "core/Time.hpp"
#include "core/Logger.hpp"
#include "database/Database.hpp"
#include "exchanger/Exchanger.hpp"

TraderApp* TraderApp::create(const Settings& settings) {
    auto* app = new TraderApp();
    if (app->init(settings)) {
        return app;
    }

    delete app;
    return nullptr;
}

TraderApp::TraderApp()
    : core::App(core::Version(1, 4, 0))
{
}

TraderApp::~TraderApp() = default;

bool TraderApp::init(const Settings& settings) {
    _settings = settings;
    return true;
}

int TraderApp::run() {
    Logger::info(util::format("TraderBot %s", _version.toString().c_str()));

    // init database
    if (not DB().init(_settings.config()))
        return EXIT_FAILURE;

    // init exchanger
    if (not Exchanger().init(_settings.config()))
        return EXIT_FAILURE;

    // init strategy
    if (not _trader.init(_settings))
        return EXIT_FAILURE;

    // run exchanger thread
    Exchanger().run();

    // run main thread
    while (_trader.isRunning()) {
        sleep_ms(100);
        Time().tick();
    }

    // stop exchanger thread
    Exchanger().stop();

    return EXIT_SUCCESS;
}
