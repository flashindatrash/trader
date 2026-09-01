//
// Created by Вадим Проскурин on 11.01.2022.
//

#include "Position.hpp"
#include "Storage.hpp"
#include "database/Database.hpp"

using namespace protocol;

static const char* identifier = "position";

std::vector<std::string> Position::keys(const std::string& username) {
    return DB().keys(Storage::pattern(username, identifier));
}

std::string Position::key(const std::string& username, const std::string& symbol) {
    return Storage::key(username, symbol, identifier);
}

Position::Position(const std::string& username, const std::string& symbol)
    : Position(key(username, symbol))
{
}

Position::Position(const std::string& key)
    : db::Object(key)
{
}