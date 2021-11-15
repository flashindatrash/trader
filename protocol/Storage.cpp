//
// Created by Вадим Проскурин on 15.11.2021.
//

#include "Storage.hpp"
#include "util/StringUtil.hpp"

using namespace protocol;

std::string Storage::key(const std::string& username, const std::string& symbol, const std::string& key) {
    return username + ":" + util::uppercase(symbol.c_str()) + ":" + key;
}