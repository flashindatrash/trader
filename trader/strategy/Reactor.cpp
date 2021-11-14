//
// Created by Вадим Проскурин on 06.11.2021.
//

#include "Reactor.hpp"
#include "Listener.hpp"
#include "database/Database.hpp"
#include "util/StringUtil.hpp"

NS_USE

Reactor* Reactor::create(const Settings& settings) {
    auto* reactor = new Reactor(settings);
    return reactor;
}

Reactor::Reactor(Settings settings)
    : _settings(std::move(settings))
{
}

bool Reactor::init() {
    return true;
}

void Reactor::pool(const Listener& listener) const {
    if (not _settings.isRelease())
        return;

    const std::string key_commands = _settings.username + ":commands";

    db::VectorValues commands = DB().lrange(key_commands);
    for (const db::Value& cmd : commands) {
        const std::string& text = cmd.asString();
        size_t delimiter_index = text.find(':');

        // delete invalid command
        if (delimiter_index == std::string::npos) {
            DB().lrem(key_commands, cmd);
            continue;
        }

        const std::string& type = text.substr(1, delimiter_index);
        const std::string& argument = text.substr(delimiter_index + 1);

        if (type == "update") {
            // check if symbol for current trader
            if (util::lowercase(argument.c_str()) == (std::string)_settings.symbol) {
                listener.sendEvent(listener.status());
                DB().lrem(key_commands, cmd);
            }
        } else {
            // delete unsupported command
            DB().lrem(key_commands, cmd);
        }
    }
}