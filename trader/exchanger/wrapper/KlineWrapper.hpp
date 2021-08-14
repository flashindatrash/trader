#pragma once

#include "exchanger/base/Storage.hpp"
#include "exchanger/base/ExchangerTypes.hpp"

class KlineWrapper : public MapIdentifier<std::string> {
public: // static
    static KlineWrapper* create();

public: // methods
    void setPrice(Price open, Price high, Price low, Price close);
    const Price& open() const;
    const Price& close() const;

    void setTime(time_t open, time_t close);
    void setTimeClose(time_t close);
    const time_t& timeClose() const;

    const double hl2() const;
    const double hlc3() const;
    const double ohlc4() const;

protected: // methods
    KlineWrapper() = default;

protected: // vars
    Price _open, _high, _low, _close = 0.0;
    time_t _time_open, _time_close = 0;
};

