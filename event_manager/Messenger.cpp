//
// Created by Вадим Проскурин on 01.11.2021.
//

#include "Messenger.hpp"
#include "Logger.hpp"
#include "database/Database.hpp"
#include "util/StringUtil.hpp"
#include <functional>

static TgBot::CurlHttpClient http_client;

Messenger::Messenger(const std::string& token)
    : _bot(token, http_client)
    , _long_pull(_bot)
    , _users("users")
{
}

bool Messenger::init() {
    Logger::info(util::format("Bot name: %s", _bot.getApi().getMe()->username.c_str()));
    if (not _bot.getApi().deleteWebhook())
        return false;

    // print all users
    for (const User& user : _users)
        Logger::info(util::format("Add user %s with id %d", user.name().c_str(), user.id()));

    // add handlers
    _bot.getEvents().onCommand("start", std::bind(&Messenger::onStart, this, std::placeholders::_1));
    _bot.getEvents().onAnyMessage(std::bind(&Messenger::onAnyMessage, this, std::placeholders::_1));
    _bot.getEvents().onCallbackQuery(std::bind(&Messenger::onCallbackQuery, this, std::placeholders::_1));
    return true;
}

void Messenger::pool() {
    for (User& user : _users) {
        const std::string key = user.name() + ":events";

        db::VectorValues events = DB().lrange(key);
        if (events.empty())
            continue;

        for (auto& event : events)
            sendMessage(user.id(), event.asString());

        DB().del(key);
    }
}

void Messenger::onStart(const TgBot::Message::Ptr message) {
    if (not StringTools::startsWith(message->text, "/start"))
        return;

    Logger::info(util::format("User start: %s", message->text.c_str()));

    auto user = _users.find_if(Users::byId(message->chat->id));
    if (user == _users.end()) {
        sendMessage(message->chat->id, util::format("Hi %s! Use /register with your trader's username", message->chat->username.c_str()));
        return;
    }

    sendMessage(message->chat->id, util::format("Hi %s! Your trader username is %s", message->chat->username.c_str(), user->name().c_str()), message->messageId);
}

void Messenger::onAnyMessage(const TgBot::Message::Ptr message) {
    Logger::info(util::format("User wrote: %s", message->text.c_str()));

    if (StringTools::startsWith(message->text, "/register")) {
        std::vector<std::string> split = StringTools::split(message->text, ' ');
        if (split.size() < 2)
            return;

        User user(split.at(1));
        user.setId(message->chat->id);

        if (_users.push(user)) {
            sendMessage(message->chat->id, util::format("Your name: %s", user.name().c_str()), message->messageId);
        } else {
            sendMessage(message->chat->id, util::format("Failed to register: %s", user.name().c_str()), message->messageId);
        }
        return;
    }

    if (StringTools::startsWith(message->text, "/")) {
        auto user = _users.find_if(Users::byId(message->chat->id));
        if (user == _users.end())
            return;

        DB().rpush(user->name() + ":commands", message->text);
    }
}

void Messenger::onCallbackQuery(const TgBot::CallbackQuery::Ptr query) {
    Logger::info(util::format("User query: %s with %s", query->message->text.c_str(), query->data.c_str()));
    sendMessage(query->message->chat->id, "Not supported", 0);
}

void Messenger::sendMessage(std::int64_t id, const std::string& message, std::int32_t reply_to, TgBot::GenericReply::Ptr reply_markup) {
    _bot.getApi().sendMessage(id, message, false, reply_to, reply_markup);
    Logger::info(util::format("Bot wrote [%d]: %s", id, message.c_str()));
}

bool Messenger::isRunning() const {
    return true;
}

void Messenger::run() {
    try {
        _long_pull.start();
    } catch (std::exception& e) {
        Logger::info(util::format("error: %s", e.what()));
        return;
    }

    pool();
}