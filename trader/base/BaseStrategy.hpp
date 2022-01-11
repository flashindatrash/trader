//
// Created by Вадим Проскурин on 11.01.2022.
//

#pragma once

#include "core/Config.hpp"

class BaseStrategy {
public: // abstract
    virtual ~BaseStrategy() = default;
    virtual bool init(const core::Config& config) = 0;
    virtual bool isRunning() const = 0;
};