//
// Created by Вадим Проскурин on 11.01.2022.
//

#pragma once

#include "Settings.hpp"

class BaseStrategy {
public: // abstract
    virtual ~BaseStrategy() = default;
    virtual bool isRunning() const = 0;
    virtual bool init(const Settings& settings) = 0;

protected: // vars
    Settings _settings;

};