//
// Created by Вадим Проскурин on 06.11.2021.
//

#include "Reactor.hpp"
#include "Listener.hpp"
#include "util/StringUtil.hpp"
#include "Command.hpp"

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

    std::vector<protocol::Command> commands = protocol::Command::get(_settings.username, _settings.symbol);
    for (const protocol::Command& command : commands) {
        switch (command.action()) {
            case protocol::Command::Update: {
                const std::string& status = listener.status();
                if (not status.empty())
                    listener.sendEvent(listener.status());
                break;
            }
            default: break;
        }
    }
}