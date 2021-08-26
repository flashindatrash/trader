#pragma once

#include "Defines.hpp"

class CandlestickWrapper;

NS_BEGIN
struct Context {
    const CandlestickWrapper* candlestick = nullptr;

    const Price& price() const;

    bool isValid() const;
};
NS_END

