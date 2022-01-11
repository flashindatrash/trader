#pragma once

#include "base/BaseStrategy.hpp"
#include "Settings.hpp"

namespace core {
    class Config;
}

namespace pair {
class Runner;
class Context;
class Algorithm;
class Listener;
class Reactor;

class Strategy : public BaseStrategy {
public: // methods
    Strategy() = default;
    ~Strategy() override;

    bool init(const core::Config& config) override;
    bool isRunning() const override;

protected: // methods
    void execute(void*);

protected: // vars
    Settings _settings;
    Runner* _runner = nullptr;
    Algorithm* _algorithm = nullptr;
    Listener* _listener = nullptr;
    Reactor* _reactor = nullptr;
};
}