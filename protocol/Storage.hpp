//
// Created by Вадим Проскурин on 15.11.2021.
//

#pragma once

#include <string>

namespace protocol {
class Storage {
public: // static
    static std::string key(const std::string& username, const std::string& key);
    static std::string key(const std::string& username, const std::string& symbol, const std::string& key);
};
}


