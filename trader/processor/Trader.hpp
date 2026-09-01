#pragma once

#include "base/Settings.hpp"

namespace trader {
class Runner;
class Context;
class Algorithm;
class Listener;

class Trader {
public: // methods
    Trader() = default;
    ~Trader();

    bool init(const Settings& settings);
    bool isRunning() const;

protected: // methods
    void execute(void*);

    Settings _settings;
    Runner* _runner = nullptr;
    Algorithm* _algorithm = nullptr;
    Listener* _listener = nullptr;
};
}
