//
// Created by Вадим Проскурин on 01.11.2021.
//

#pragma once

#include <tgbot/tgbot.h>

class Messenger {
public: // methods
    Messenger(const std::string& token);

    bool isRunning() const;

    bool init();
    void run();

    void sendMessage(std::int64_t id, const std::string& message);
    void sendMessage(const std::string& username, const std::string& message);

private: // methods
    void onCommand(const TgBot::Message::Ptr message);
    void onAnyMessage(const TgBot::Message::Ptr message);

private: // vars
    TgBot::Bot _bot;
    TgBot::TgLongPoll _long_pull;

    // table id/username
    std::unordered_map<std::int64_t, std::string> _users;
};


