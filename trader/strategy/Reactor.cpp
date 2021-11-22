//
// Created by Вадим Проскурин on 06.11.2021.
//

#include "Reactor.hpp"
#include "Context.hpp"
#include "Command.hpp"
#include "Algorithm.hpp"
#include "Formatter.hpp"
#include "Event.hpp"
#include "Position.hpp"

NS_USE

Reactor* Reactor::create(Algorithm& algorithm, const Settings& settings) {
    auto* reactor = new Reactor(algorithm, settings);
    return reactor;
}

Reactor::Reactor(Algorithm& algorithm, Settings settings)
    : _algorithm(algorithm)
    , _settings(std::move(settings))
{
}

bool Reactor::init() {
    return true;
}

void Reactor::execute() const {
    if (not _settings.isRelease() || Context::current == nullptr)
        return;

    std::vector<protocol::Command> commands = protocol::Command::get(_settings.username, _settings.symbol);
    for (const protocol::Command& command : commands) {
        switch (command.action()) {
            case protocol::Command::Update: {
                if (_algorithm.position().has()) {
                    Formatter event = Formatter::update(_algorithm.position(), *Context::current);
                    protocol::Event::add(_settings.username, event.html());
                }
                break;
            }
            case protocol::Command::Stats: {
                // listener.sendEvent(listener.statistics());
                break;
            }
            case protocol::Command::Close: {
                _algorithm.tryClose();
                break;
            }
            default: break;
        }
    }
}