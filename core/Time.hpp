#pragma once

#include "Proxy.hpp"
#include "Signal.hpp"

class Timer : public core::Proxy<Timer> {
public: // static
    static const time_t sSecond;
    static const time_t sMinute;
    static const time_t sHour;
    static const time_t sDay;

public: // methods
    Timer() = default;

    void tick();

    void set(time_t ms);

    time_t ms() const;
    time_t sec() const;

public: // signals
    Signal<time_t> onTick;

protected: // vars
    time_t _local = 0;
};

#define Time() Timer::getInstance()
