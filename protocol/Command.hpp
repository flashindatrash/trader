//
// Created by Вадим Проскурин on 14.11.2021.
//

#pragma once

#include <string>

namespace protocol {

class Command {
public: // enum
    enum Action : unsigned int {
        Invalid,
        Update,
        Stats,
        Close
    };

private: // static
    static std::string key(const std::string& username, const std::string& symbol);

public: // static
    static bool add(const std::string& username, const std::string& symbol, Action action);
    static std::vector<Command> get(const std::string& username, const std::string& symbol);
    static Action parse(const std::string& value);

public: // methods
    Command(Action action);

    Action action() const;

private: // vars
    Action _action = Invalid;

};

}

