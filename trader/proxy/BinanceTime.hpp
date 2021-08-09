#pragma once

#include "Proxy.hpp"
#include <global.hpp>

class BinanceTime : public core::Proxy<BinanceTime>
{
public: // static
    static const time_t sSecond;
    static const time_t sMinute;
    static const time_t sHour;
    static const time_t sDay;

public: // methods
    BinanceTime() {}

    void init();
    time_t getCurrent() const;
};

#define STime() BinanceTime::getInstance()
