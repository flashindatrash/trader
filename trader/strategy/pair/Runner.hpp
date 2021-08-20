#pragma once

#include "Defines.hpp"
#include "Signal.hpp"
#include "exchanger/base/Symbol.hpp"

NS_BEGIN
class Settings;

class Runner {
public: // static
    static Runner* create(const Settings& settings);

public: // methods
    void run();
    bool isRunning() const;

protected: // methods
    Runner(const Symbol& symbol, bool test);

    void tick(time_t ms);

public: // signals
    Signal<time_t> onTick;

protected: // vars
    const bool _test;
    const Symbol _symbol;
    bool _active;
};
NS_END

