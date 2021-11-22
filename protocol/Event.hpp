//
// Created by Вадим Проскурин on 21.11.2021.
//


#pragma once

#include <string>

namespace protocol {

class Event {
private: // static
    static std::string key(const std::string& username);

public: // static
    static bool add(const std::string& username, const std::string& text);
    static std::vector<Event> get(const std::string& username);

public: // methods
    Event(std::string text);

    const std::string& text() const;

private: // vars
    std::string _text;
};

}