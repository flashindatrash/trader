//
// Created by Вадим Проскурин on 01.11.2021.
//

#include "Messenger.hpp"
#include <functional>

Messenger::Messenger(const std::string& token)
    : _bot(token)
    , _long_pull(_bot)
{
}

bool Messenger::init() {
    std::vector<TgBot::BotCommand::Ptr> commands;
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = "log";
    cmdArray->description = "receive logs";
    commands.push_back(cmdArray);
    if (not _bot.getApi().setMyCommands(commands))
        return false;

    printf("Bot username: %s\n", _bot.getApi().getMe()->username.c_str());
    if (not _bot.getApi().deleteWebhook())
        return false;

    _bot.getEvents().onCommand("start", std::bind(&Messenger::onCommand, this, std::placeholders::_1));
    _bot.getEvents().onAnyMessage(std::bind(&Messenger::onAnyMessage, this, std::placeholders::_1));
    return true;
}

void Messenger::onCommand(const TgBot::Message::Ptr message) {
    _bot.getApi().sendMessage(message->chat->id, "Hi!");
}

void Messenger::onAnyMessage(const TgBot::Message::Ptr message) {
    printf("User wrote %s\n", message->text.c_str());
    if (StringTools::startsWith(message->text, "/start"))
        return;

    _bot.getApi().sendMessage(message->chat->id, "Your message is: " + message->text);
}

bool Messenger::isRunning() const {
    return true;
}

void Messenger::run() {
    _long_pull.start();
}