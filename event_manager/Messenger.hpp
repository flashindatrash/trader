//
// Created by Вадим Проскурин on 01.11.2021.
//

#pragma once

#include <tgbot/tgbot.h>

class Messenger {
public: // methods
    Messenger(const std::string& token);

    bool init();
    void run();

private: // vars
    TgBot::Bot _bot;
};


