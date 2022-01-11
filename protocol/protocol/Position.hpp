//
// Created by Вадим Проскурин on 11.01.2022.
//

#pragma once

#include <string>

namespace protocol {

class Position {
public: // static
    static std::string key(const std::string& username, const std::string& symbol);
};

}

