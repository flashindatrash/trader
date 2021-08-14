#pragma once

#include "exchanger/base/ExchangerTypes.hpp"

class KlineWrapper {
public: // static
    static KlineWrapper* create();

public: // methods
    const Price& open() const;
    const Price& close() const;

    void setPrice(Price open, Price high, Price low, Price close);
    void setTime(time_t open, time_t close);

    const time_t& timeClose() const;

protected: // methods
    KlineWrapper() = default;

protected: // vars
    Price _open, _high, _low, _close = 0.0;
    time_t _time_open, _time_close = 0;
};

