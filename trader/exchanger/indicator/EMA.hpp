//
// Created by Вадим Проскурин on 03.10.2021.
//

#pragma once

#include "BaseIndicator.hpp"

class EMA : public BaseIndicator {
public: // methods
    EMA() = default;
    EMA(size_t length);

    bool empty() const;
    size_t size() const;

    Price last() const;
    Price prev() const;

    bool load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) override;

protected: // vars
    size_t _length = 0;
    std::vector<Price> _data;
};


