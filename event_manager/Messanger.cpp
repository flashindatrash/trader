//
// Created by Вадим Проскурин on 01.11.2021.
//

#include "Messenger.hpp"

Messenger::Messenger(const std::string& token)
    : _bot(token)
{
}

bool Messenger::init() {
    std::vector<TgBot::BotCommand::Ptr> commands;
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = "log";
    cmdArray->description = "receive logs";
    commands.push_back(cmdArray);
    _bot.getApi().setMyCommands(commands);

    printf("Bot username: %s\n", bot.getApi().getMe()->username.c_str());
    _bot.getApi().deleteWebhook();

    _bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {
        bot.getApi().sendMessage(message->chat->id, "Hi!");
    });
    _bot.getEvents().onAnyMessage([&_bot](TgBot::Message::Ptr message) {
        printf("User wrote %s\n", message->text.c_str());
        if (StringTools::startsWith(message->text, "/start")) {
            return;
        }

        _bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
    });

    return true;
}

void Messenger::run() {
    TgBot::TgLongPoll longPoll(bot);
    while (true) {
        longPoll.start();
    }
}