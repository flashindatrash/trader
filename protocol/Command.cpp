//
// Created by Вадим Проскурин on 14.11.2021.
//

#include "Command.hpp"
#include "Storage.hpp"
#include "util/StringUtil.hpp"
#include <utility>

using namespace protocol;

std::string Command::key(const std::string& username, const std::string& symbol) {
    return Storage::key(username, symbol, "commands");
}

bool Command::add(const std::string& username, const std::string& symbol, Action action) {
    if (action == Invalid)
        return false;

    return DB().rpush(key(username, symbol), (int)action) > 0;
}

std::vector<Command> Command::get(const std::string& username, const std::string& symbol) {
    std::string storage = key(username, symbol);

    std::vector<Command> commands;
    db::VectorValues values = DB().lrange(storage);
    for (const db::Value& value : values)
        commands.emplace_back((Action)value.asInt());

    if (not values.empty())
        DB().del(storage);

    return commands;
}

Command::Action Command::parse(const std::string& value) {
    std::string lowercase = util::lowercase(value.c_str());
    if (lowercase == "update")
        return Update;
    else if (lowercase == "stats")
        return Stats;
    
    return Invalid;
}

Command::Command(Action action)
    : _action(action)
{
}

Command::Action Command::action() const {
    return _action;
}