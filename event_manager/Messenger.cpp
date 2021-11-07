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
    // add telegram commands
    std::vector<TgBot::BotCommand::Ptr> commands;

    TgBot::BotCommand::Ptr cmdRegister(new TgBot::BotCommand);
    cmdRegister->command = "register";
    cmdRegister->description = "Register user";
    commands.push_back(cmdRegister);

    TgBot::BotCommand::Ptr cmdSelect(new TgBot::BotCommand);
    cmdSelect->command = "select";
    cmdSelect->description = "Choose a pair of trader";
    commands.push_back(cmdSelect);

    if (not _bot.getApi().setMyCommands(commands))
        return false;

    Logger::info(util::format("Bot name: %s", _bot.getApi().getMe()->username.c_str()));
    if (not _bot.getApi().deleteWebhook())
        return false;

    // print all users
    for (const User& user : _users)
        Logger::info(util::format("Add user %s with id %d", user.name().c_str(), user.id()));

    // add handlers
    _bot.getEvents().onCommand("start", std::bind(&Messenger::onStart, this, std::placeholders::_1));
    _bot.getEvents().onCommand("select", std::bind(&Messenger::onSelect, this, std::placeholders::_1));
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

    if (not checkRegistration(message->chat))
        return;

    auto it = _users.find_if(Users::byId(message->chat->id));
    sendMessage(message->chat->id, util::format("Hi %s! Your trader username is %s", message->chat->username.c_str(), it->name().c_str()), message->messageId);
}

void Messenger::onSelect(const TgBot::Message::Ptr message) {
    if (not StringTools::startsWith(message->text, "/select"))
        return;

    Logger::info(util::format("User select: %s", message->text.c_str()));

    if (not checkRegistration(message->chat))
        return;

    _bot.getApi().sendChatAction(message->chat->id, "typing");
}

void Messenger::onAnyMessage(const TgBot::Message::Ptr message) {
    std::int64_t id = message->chat->id;
    const std::string& text = message->text;
    const TgBot::Message::Ptr& reply_to = message->replyToMessage;

    Logger::info(util::format("User wrote: %s", text.c_str()));

    static bool wait_username = false;
    if (StringTools::startsWith(text, "/register")) {
        wait_username = true;
        sendMessage(id, "Input trader name", message->messageId, std::make_shared<TgBot::ForceReply>());
        return;
    }

    if (wait_username) {
        User user(text);
        user.setId(id);

        if (_users.push(user)) {
            sendMessage(id, util::format("Your name: %s", user.name().c_str()), message->messageId);
        } else {
            sendMessage(id, util::format("Failed to register: %s", user.name().c_str()), message->messageId);
        }

        wait_username = false;
    }

    if (StringTools::startsWith(text, "/select")) {
        sendMessage(id, "test force reply", message->messageId, std::make_shared<TgBot::ForceReply>());
    }

    if (reply_to.use_count()) {
        Logger::info(util::format("reply to %s", reply_to->text.c_str()));
    }

    if (StringTools::startsWith(text, "/test")) {
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

void Messenger::onCallbackQuery(const TgBot::CallbackQuery::Ptr query) {
    Logger::info(util::format("User query: %s with %s", query->message->text.c_str(), query->data.c_str()));
    sendMessage(query->message->chat->id, "ok", 0);
}

void Messenger::sendMessage(std::int64_t id, const std::string& message, std::int32_t reply_to, TgBot::GenericReply::Ptr reply_markup) {
    _bot.getApi().sendMessage(id, message, false, reply_to, reply_markup);
    Logger::info(util::format("Bot wrote [%d]: %s", id, message.c_str()));
}

bool Messenger::checkRegistration(const TgBot::Chat::Ptr& chat) {
    auto it = _users.find_if(Users::byId(chat->id));

    if (it == _users.end()) {
        sendMessage(chat->id, util::format("Hi %s! Use /register with your trader's username", chat->username.c_str()));
        return false;
    }

    return true;
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