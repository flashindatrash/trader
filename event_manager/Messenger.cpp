//
// Created by Вадим Проскурин on 01.11.2021.
//

#include "Messenger.hpp"
#include "database/Database.hpp"
#include <functional>

static TgBot::CurlHttpClient http_client;

Messenger::Messenger(const std::string& token)
    : _bot(token, http_client)
    , _long_pull(_bot)
{
}

bool Messenger::init() {
    std::vector<TgBot::BotCommand::Ptr> commands;
    TgBot::BotCommand::Ptr cmdArray(new TgBot::BotCommand);
    cmdArray->command = "register";
    cmdArray->description = "Register trader";
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
    std::int64_t id = message->chat->id;

    printf("User wrote %s\n", message->text.c_str());
    if (StringTools::startsWith(message->text, "/start"))
        return;

    if (StringTools::startsWith(message->text, "/register")) {
        _users.insert(std::make_pair(id, ""));
        sendMessage(id, "Input trader name", message->messageId);
        return;
    }

    auto user = _users.find(id);
    if (user == _users.end())
        return;

    std::string& nickname = user->second;
    if (nickname.empty() && not message->text.empty()) {
        nickname = message->text;
        sendMessage(nickname, "Your username: " + nickname, message->messageId);
    }
}

void Messenger::sendMessage(std::int64_t id, const std::string& message, std::int32_t reply_to) {
    _bot.getApi().sendMessage(id, message, false, reply_to);
}

void Messenger::sendMessage(const std::string& username, const std::string& message, std::int32_t reply_to) {
    for (auto& it : _users)
        if (it.second == username)
            return sendMessage(it.first, message, reply_to);
}

bool Messenger::isRunning() const {
    return true;
}

void Messenger::run() {
    for (auto& user : _users) {
        const std::string& username = user.second;
        if (username.empty())
            continue;

        const std::string key = username + ":events";

        db::VectorValues events = DB().lrange(key);
        if (events.empty())
            continue;

        for (auto& event : events)
            sendMessage(username, event.asString());

        DB().del(key);
    }

    _long_pull.start();
}