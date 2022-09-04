//
// Created by Вадим Проскурин on 11.01.2022.
//

#pragma once

#include "database/Object.hpp"

namespace protocol {

class Position : public db::Object {
protected: // static
    static std::string key(const std::string& username, const std::string& symbol);
    static std::vector<std::string> keys(const std::string& username);

public: // methods
    Position(const std::string& username, const std::string& symbol);
    Position() = default;

private: // methods
    Position(const std::string& key);
};

}

