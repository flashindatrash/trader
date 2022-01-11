//
// Created by Вадим Проскурин on 11.01.2022.
//

#pragma once

#include "database/Object.hpp"

namespace protocol {

class Position : public db::Object {
private: // static
    static std::string key(const std::string& username, const std::string& symbol);

public: // methods
    Position(const std::string& username, const std::string& symbol);
    Position() = default;

};

}

