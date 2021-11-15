//
// Created by Вадим Проскурин on 14.11.2021.
//

#include "Command.hpp"

#include <utility>
#include "util/StringUtil.hpp"

using namespace protocol;

bool Command::add(const std::string& username, const std::string& symbol, Action action) {
    if (action == Invalid)
        return false;

    std::string symbol_lowercase = util::lowercase(symbol.c_str());
    return DB().rpush(username + ":" + symbol_lowercase + ":commands", (int)action) > 0;
}

std::vector<Command> Command::get(const std::string& username, const std::string& symbol) {
    std::vector<Command> commands;
    db::VectorValues values = DB().lrange(username + ":" + symbol + ":commands");
    for (const db::Value& value : values)
        commands.emplace_back((Action)value.asInt());

    if (not values.empty())
        DB().del(username + ":" + symbol + ":commands");

    return commands;
}

Command::Action Command::parse(const std::string& value) {
    std::string lowercase = util::lowercase(value.c_str());
    if (lowercase == "update")
        return Update;

    return Invalid;
}

Command::Command(Action action)
    : _action(action)
{
}

Command::Action Command::action() const {
    return _action;
}