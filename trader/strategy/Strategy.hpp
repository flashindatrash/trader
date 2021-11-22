#pragma once

#include "Defines.hpp"

namespace core {
    class Config;
}

NS_BEGIN
class Runner;
class Context;
class Algorithm;
class Listener;
class Reactor;
NS_END

class Strategy {
public: // static
    static Strategy* create(const core::Config& config);

public: // methods
    ~Strategy();

    bool init(const core::Config& config);
    bool isRunning() const;

protected: // methods
    Strategy() = default;

    void execute(void*);

protected: // vars
    NS::Runner* _runner = nullptr;
    NS::Algorithm* _algorithm = nullptr;
    NS::Listener* _listener = nullptr;
    NS::Reactor* _reactor = nullptr;
};
