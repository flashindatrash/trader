//
// Created by Вадим Проскурин on 29.10.2021.
//

#pragma once

#include "EMA.hpp"

namespace indicator {
class MACD : public Base {
public: // methods
    MACD() = default;
    MACD(size_t fast, size_t slow, size_t signal);

    [[nodiscard]] OrderSide trend() const;
    [[nodiscard]] OrderSide signal() const;

    [[nodiscard]] bool crossed() const;
    [[nodiscard]] bool empty() const;

    bool load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) override;

private: // vars
    EMA _fast;
    EMA _slow;
    EMA _signal;
};
}

