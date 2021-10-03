//
// Created by Вадим Проскурин on 03.10.2021.
//

#pragma once

#include "exchanger/wrapper/ChartWrapper.hpp"

class EMA {
public: // methods
    EMA() = default;
    EMA(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end, size_t length);

    bool empty() const;
    size_t size() const;

    Price last() const;
    Price prev() const;

protected: // vars
    std::vector<Price> _data;
};


