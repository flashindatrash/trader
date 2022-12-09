//
// Created by Вадим Проскурин on 03.10.2021.
//

#pragma once

#include "BaseIndicator.hpp"

namespace indicator {
class EMA : public Base {
public: // methods
    EMA() = default;
    explicit EMA(size_t length);

    [[nodiscard]] bool empty() const;
    [[nodiscard]] size_t size() const;

    [[nodiscard]] Price last() const;
    [[nodiscard]] Price prev() const;

    void add(Price value);

    [[nodiscard]] const Price& at(size_t index) const;

    bool load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) override;

protected: // methods
    [[nodiscard]] double multiplier() const;

protected: // vars
    size_t _length = 0;
    std::vector<Price> _data;
};
}

