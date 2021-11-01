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

private: // methods
    void onCommand(const TgBot::Message::Ptr message);
    void onAnyMessage(const TgBot::Message::Ptr message);

private: // vars
    TgBot::Bot _bot;
    TgBot::TgLongPoll _long_pull;
};


