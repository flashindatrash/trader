//
// Created by Вадим Проскурин on 03.10.2021.
//

#pragma once

#include "EMA.hpp"

namespace indicator {
class DEMA : public Base {
public: // methods
    DEMA() = default;
    DEMA(size_t fast, size_t slow);

    [[nodiscard]] OrderSide trend() const;
    [[nodiscard]] OrderSide signal() const;

    [[nodiscard]] bool crossed() const;
    [[nodiscard]] bool empty() const;

    bool load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) override;

protected: // static
    static OrderSide compare(Price fast, Price slow);

protected: // vars
    EMA _fast;
    EMA _slow;
};
}

