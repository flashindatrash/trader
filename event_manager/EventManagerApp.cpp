//
// Created by Вадим Проскурин on 01.11.2021.
//

#include "EventManagerApp.hpp"
#include "Logger.hpp"
#include "Messenger.hpp"

EventManagerApp* EventManagerApp::create(const core::Config& config) {
    auto* app = new EventManagerApp(config);
    return app;
}

EventManagerApp::EventManagerApp(const core::Config& config)
    : core::HttpApp(config)
{
}

int EventManagerApp::run() {
    const std::string& token = _config.asString("EVENT_TOKEN");
    if (token.empty()) {
        Logger::info("Config: unknown EVENT_TOKEN");
        return EXIT_FAILURE;
    }


    Messenger messenger(token);
    if (not messenger.init()) {
        Logger::info("can't init messenger");
        return EXIT_FAILURE;
    }

    messenger.run();
    return EXIT_SUCCESS;
}