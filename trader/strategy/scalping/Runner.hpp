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
    bool start(const Settings& settings);
    bool isRunning() const;
    void setCallback(Callback::Fn callback);

protected: // methods
    Runner() = default;

    void dispatch(const Context& context);
    void tick(time_t ms);

protected: // vars
    Callback _dispatcher;
    bool _active = false;
    ChartWrapper* _chart = nullptr;
};
NS_END

