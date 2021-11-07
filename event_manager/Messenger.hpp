//
// Created by Вадим Проскурин on 01.11.2021.
//

#pragma once

#include <tgbot/tgbot.h>
#include "User.hpp"

class Messenger {
public: // methods
    Messenger(const std::string& token);

    bool isRunning() const;

    bool init();
    void run();

    void sendMessage(std::int64_t id, const std::string& message, std::int32_t reply_to = 0, TgBot::GenericReply::Ptr reply_markup = std::make_shared<TgBot::GenericReply>());
    void sendMessage(const std::string& username, const std::string& message, std::int32_t reply_to = 0, TgBot::GenericReply::Ptr reply_markup = std::make_shared<TgBot::GenericReply>());

private: // methods
    void onCommand(const TgBot::Message::Ptr message);
    void onAnyMessage(const TgBot::Message::Ptr message);

    void check();

private: // vars
    TgBot::Bot _bot;
    TgBot::TgLongPoll _long_pull;
    Users _users;
};


