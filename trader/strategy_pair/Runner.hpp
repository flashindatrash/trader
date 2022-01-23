#pragma once

#include "core/Signal.hpp"

class ChartWrapper;
class Settings;

namespace pair {
class Context;

class Runner {
public: // static
    typedef Signal<void*> Callback;

    static Runner* create();

public: // methods
    bool start(const Settings& settings);
    void setCallback(Callback::Fn callback);

protected: // methods
    Runner() = default;

    void tick(time_t ms);
    void setContext(const Context& context);

protected: // vars
    Callback _dispatcher;
    ChartWrapper* _chart = nullptr;
};
}

