//
// Created by Вадим Проскурин on 01.11.2021.
//

#include "Messenger.hpp"
#include "Logger.hpp"
#include "database/Database.hpp"
#include "util/StringUtil.hpp"
#include <functional>
#include "Command.hpp"
#include "Event.hpp"

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
        std::vector<protocol::Event> events = protocol::Event::get(user.name());
        for (auto& event : events)
            sendMessage(user.id(), event.text());
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

    // supports only commands with /
    if (not StringTools::startsWith(message->text, "/")) {
        sendMessage(message->chat->id, "Start message with a /", message->messageId);
        return;
    }

    // split by spaces
    std::vector<std::string> split = StringTools::split(message->text, ' ');

    // supports only 1 argument
    if (split.size() > 2) {
        sendMessage(message->chat->id, "Invalid command", message->messageId);
        return;
    }

    std::string command = split.at(0);
    std::string argument = split.size() > 1 ? split.at(1) : "";

    // register new username
    if (command == "/register") {
        std::string response;

        if (argument.empty()) {
            response = "Username is empty";
        } else {
            User user(argument);
            user.setId(message->chat->id);

            if (_users.push(user))
                response = util::format("Your name: %s", user.name().c_str());
            else
                response = util::format("Failed to register: %s", user.name().c_str());
        }

        sendMessage(message->chat->id, response, message->messageId);
        return;
    }

    // send other commands into trader
    auto user = _users.find_if(Users::byId(message->chat->id));
    if (user == _users.end())
        return;

    // format: /pair:action
    std::string pair = command.substr(1);
    if (protocol::Command::add(user->name(), pair, protocol::Command::parse(argument))) {
        Logger::info(util::format("Add command %s into %s (user: %s)", argument.c_str(), pair.c_str(), user->name().c_str()));
    } else {
        Logger::info(util::format("Failed add command %s into %s (user: %s)", argument.c_str(), pair.c_str(), user->name().c_str()));
    }
}

void Messenger::onCallbackQuery(const TgBot::CallbackQuery::Ptr query) {
    Logger::info(util::format("User query: %s with %s", query->message->text.c_str(), query->data.c_str()));
    sendMessage(query->message->chat->id, "Not supported", 0);
}

void Messenger::sendMessage(std::int64_t id, const std::string& message, std::int32_t reply_to, TgBot::GenericReply::Ptr reply_markup) {
    _bot.getApi().sendMessage(id, message, false, reply_to, reply_markup, "HTML");
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