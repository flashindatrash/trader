//
// Created by Вадим Проскурин on 11.01.2022.
//

#include "Position.hpp"
#include "Storage.hpp"

using namespace protocol;

std::string Position::key(const std::string& username, const std::string& symbol) {
    return Storage::key(username, symbol, "position");
}