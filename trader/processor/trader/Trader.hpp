#pragma once

#include "processor/Processor.hpp"

namespace trader {
class Runner;
class Context;
class Algorithm;
class Listener;
class Reactor;

class Trader : public Processor {
public: // methods
    Trader() = default;
    ~Trader() override;

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