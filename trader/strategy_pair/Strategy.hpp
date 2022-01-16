#pragma once

#include "base/BaseStrategy.hpp"

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

    bool init(const Settings& settings) override;
    bool isRunning() const override;

protected: // methods
    void execute(void*);

    Runner* _runner = nullptr;
    Algorithm* _algorithm = nullptr;
    Listener* _listener = nullptr;
    Reactor* _reactor = nullptr;
};
}