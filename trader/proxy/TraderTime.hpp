#pragma once

#include "Proxy.hpp"
#include "Signal.hpp"

class TraderTime : public core::Proxy<TraderTime>
{
public: // static
    static const time_t sSecond;
    static const time_t sMinute;
    static const time_t sHour;
    static const time_t sDay;

public: // methods
    TraderTime() = default;

    void tick();
    time_t ms() const;

public: // signals
    Signal<time_t> onTick;
};

#define Time() TraderTime::getInstance()
