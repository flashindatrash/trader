#pragma once

#include "Defines.hpp"

class CandlestickWrapper;

NS_BEGIN
struct Context {
    time_t time = 0;
    const CandlestickWrapper* candlestick = nullptr;

    const Price& price() const;

    bool isValid() const;
};
NS_END

