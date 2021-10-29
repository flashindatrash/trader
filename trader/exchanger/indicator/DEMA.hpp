//
// Created by Вадим Проскурин on 03.10.2021.
//

#pragma once

#include "EMA.hpp"

class DEMA : public BaseIndicator {
public: // methods
    DEMA() = default;
    DEMA(size_t fast, size_t slow);

    OrderSide trend() const;
    OrderSide signal() const;

    bool crossed() const;
    bool empty() const;

    bool load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) override;

protected: // static
    static OrderSide compare(Price fast, Price slow);

protected: // vars
    EMA _fast;
    EMA _slow;
};


