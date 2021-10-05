//
// Created by Вадим Проскурин on 03.10.2021.
//

#pragma once

#include "EMA.hpp"

class EMACross {
public: // methods
    EMACross() = default;
    EMACross(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end, size_t long_length, size_t short_length);

    OrderSide trend() const;

    bool crossed() const;
    bool empty() const;

protected: // static
    static OrderSide compare(Price ema_long, Price ema_short) ;

protected: // vars
    EMA _long;
    EMA _short;
};


