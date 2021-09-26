#pragma once

#include "Defines.hpp"
#include "Signal.hpp"

class ChartWrapper;

NS_BEGIN
class Settings;
class Context;

class Runner {
public: // static
    typedef Signal<Context> Callback;

    static Runner* create();

public: // methods
    void start(const Settings& settings);
    bool isActive() const;
    void setCallback(Callback::Fn callback);

protected: // methods
    Runner() = default;

    void tick(time_t ms);

protected: // vars
    Callback _dispatcher;
    bool _active = false;
    ChartWrapper* _chart = nullptr;
};
NS_END

