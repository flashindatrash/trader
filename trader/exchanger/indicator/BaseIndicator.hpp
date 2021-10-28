//
// Created by Вадим Проскурин on 28.10.2021.
//

#pragma once

#include "exchanger/wrapper/ChartWrapper.hpp"

class BaseIndicator {
public:
    virtual bool load(ChartWrapper::ConstIterator begin, ChartWrapper::ConstIterator end) = 0;
};


