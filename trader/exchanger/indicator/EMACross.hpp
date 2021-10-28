//
// Created by Вадим Проскурин on 03.10.2021.
//

#pragma once

#include "EMA.hpp"

class EMACross : public BaseIndicator {
public: // methods
    EMACross() = default;
    EMACross(size_t long_length, size_t short_length);

    OrderSide trend() const;

    bool crossed() const;
    bool empty() const;

    bool load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) override;

protected: // static
    static OrderSide compare(Price ema_long, Price ema_short) ;

protected: // vars
    EMA _long;
    EMA _short;
};


