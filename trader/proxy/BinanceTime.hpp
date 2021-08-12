#pragma once

#include "Proxy.hpp"
#include <global.hpp>

class BinanceTime : public core::Proxy<BinanceTime>, public core::Emitter<time_t>
{
public: // static
    static const time_t sSecond;
    static const time_t sMinute;
    static const time_t sHour;
    static const time_t sDay;

public: // methods
    BinanceTime() = default;

    void init();
    void tick();
    time_t getCurrent() const;
};

#define STime() BinanceTime::getInstance()
