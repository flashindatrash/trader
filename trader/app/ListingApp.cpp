//
// Created by Вадим Проскурин on 06.01.2022.
//

#include "ListingApp.hpp"
#include "Logger.hpp"
#include "Event.hpp"
#include "Time.hpp"
#include "database/Database.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Symbol.hpp"

ListingApp* ListingApp::create(const core::Config& config) {
    auto* app = new ListingApp(config);
    return app;
}

ListingApp::ListingApp(const core::Config& config)
    : BaseApp(config, core::Version(1, 0, 0))
{
}

ListingApp::~ListingApp() {
}

int ListingApp::run() {
    Logger::info(util::format("ListingBot %s", _version.toString().c_str()));

    // init database
    if (not DB().init(_config))
        return EXIT_FAILURE;

    // init exchanger
    if (not Exchanger().init(_config))
        return EXIT_FAILURE;

    Symbol::onAdded.connect(std::bind(&ListingApp::onSymbolAdded, this, std::placeholders::_1));

    // run exchanger thread
    Exchanger().run();

    // run main thread
    while (true) {
        sleep_ms(60000);

        if (not Exchanger().loadPairs())
            break;

        Time().tick();
    }

    // stop exchanger thread
    Exchanger().stop();

    return EXIT_SUCCESS;
}

void ListingApp::onSymbolAdded(const Symbol& symbol) {
    std::string text = util::format("New listing %s", symbol.c_str());
    Logger::info(text);

    std::string username = _config.asString("REDIS_USERNAME");
    protocol::Event::add(username, text);
}