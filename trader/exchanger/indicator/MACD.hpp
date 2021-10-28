//
// Created by Вадим Проскурин on 29.10.2021.
//

#pragma once

#include "EMA.hpp"

class MACD : public BaseIndicator {
public:
    MACD() = default;
    MACD(size_t fast, size_t slow, size_t signal);

    bool load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) override;

private:
    EMA _fast;
    EMA _slow;
    EMA _signal;
};


