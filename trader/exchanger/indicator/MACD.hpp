//
// Created by Вадим Проскурин on 29.10.2021.
//

#pragma once

#include "EMA.hpp"

class MACD : public BaseIndicator {
public: // methods
    MACD() = default;
    MACD(size_t fast, size_t slow, size_t signal);

    OrderSide trend() const;
    OrderSide signal() const;

    bool crossed() const;
    bool empty() const;

    bool load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) override;

private: // vars
    EMA _fast;
    EMA _slow;
    EMA _signal;
};


