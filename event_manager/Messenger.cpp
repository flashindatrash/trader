//
// Created by Вадим Проскурин on 01.11.2021.
//

#include "Messenger.hpp"
#include "database/Database.hpp"
#include "Logger.hpp"
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
    // add telegram commands
    std::vector<TgBot::BotCommand::Ptr> commands;

    TgBot::BotCommand::Ptr cmdRegister(new TgBot::BotCommand);
    cmdRegister->command = "register";
    cmdRegister->description = "Register trader";
    commands.push_back(cmdRegister);

    TgBot::BotCommand::Ptr cmdTest(new TgBot::BotCommand);
    cmdTest->command = "test";
    cmdTest->description = "Test";
    commands.push_back(cmdTest);

    if (not _bot.getApi().setMyCommands(commands))
        return false;

    Logger::info(util::format("Bot name: %s", _bot.getApi().getMe()->username.c_str()));
    if (not _bot.getApi().deleteWebhook())
        return false;

    // notify users about restart
    for (const User& user : _users) {
        Logger::info(util::format("Add user %s with id %d", user.name().c_str(), user.id()));
        sendMessage(user.id(), "Bot restarted");
    }

    // add handlers
    _bot.getEvents().onCommand("start", std::bind(&Messenger::onCommand, this, std::placeholders::_1));
    _bot.getEvents().onAnyMessage(std::bind(&Messenger::onAnyMessage, this, std::placeholders::_1));
    return true;
}

void Messenger::check() {
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

void Messenger::onCommand(const TgBot::Message::Ptr message) {
    std::int64_t id = message->chat->id;

    _bot.getApi().sendChatAction(id, "typing");

    if (StringTools::startsWith(message->text, "/start")) {
        auto it = _users.find_if(Users::byId(id));

        std::string reply;
        if (it == _users.end())
            reply = util::format("Hi %s! Use /register with your trader's username", message->chat->username.c_str());
        else
            reply = util::format("Hi %s! Your trader username is %s", message->chat->username.c_str(), it->name().c_str());

        sendMessage(id, reply, message->messageId);
    }

    if (StringTools::startsWith(message->text, "/test")) {
        TgBot::InlineKeyboardMarkup::Ptr markup(new TgBot::InlineKeyboardMarkup());

        std::vector<TgBot::InlineKeyboardButton::Ptr> row;
        TgBot::InlineKeyboardButton::Ptr btn(new TgBot::InlineKeyboardButton);
        btn->text = "text";
        btn->callbackData = "callback data";
        row.push_back(btn);

        markup->inlineKeyboard.push_back(row);

        sendMessage(id, "test", message->messageId, markup);
    }
}

void Messenger::onAnyMessage(const TgBot::Message::Ptr message) {
    std::int64_t id = message->chat->id;
    const std::string& text = message->text;

    Logger::info(util::format("User wrote: %s", text.c_str()));

    static bool wait_username = false;
    if (StringTools::startsWith(text, "/register")) {
        wait_username = true;
        sendMessage(id, "Input trader name", message->messageId);
        return;
    }

    if (wait_username) {
        User user(text);
        user.setId(id);

        if (_users.push(user)) {
            sendMessage(id, "Your name: " + user.name(), message->messageId);
        } else {
            sendMessage(id, "Failed to register: " + user.name(), message->messageId);
        }

        wait_username = false;
    }
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

    check();
}