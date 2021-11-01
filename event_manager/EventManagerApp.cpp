//
// Created by Вадим Проскурин on 01.11.2021.
//

#include "EventManagerApp.hpp"
#include "Logger.hpp"
#include <tgbot/tgbot.h>

EventManagerApp* EventManagerApp::create(const core::Config& config) {
    EventManagerApp* app = new EventManagerApp(config);
    return app;
}

EventManagerApp::EventManagerApp(const core::Config& config)
    : core::HttpApp(config)
{
}

int EventManagerApp::run() {
    const std::string& token = _config.asString("EVENT_TOKEN");
    if (token.empty()) {
        Logger::info("undefined EVENT_TOKEN");
        return EXIT_FAILURE;
    }

    TgBot::Bot bot(token);
    std::vector<TgBot::BotCommand::Ptr> commands;
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = "log";
    cmdArray->description = "receive logs";
    commands.push_back(cmdArray);
    bot.getApi().setMyCommands(commands);

    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }

        bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });

    printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
    bot.getApi().deleteWebhook();

    TgBot::TgLongPoll longPoll(bot);
    while (true) {
        longPoll.start();
    }

    return EXIT_SUCCESS;
}