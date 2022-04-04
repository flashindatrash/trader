//
// Created by Вадим Проскурин on 11.01.2022.
//

#pragma once

#include "base/Settings.hpp"

class Processor {
public: // static
    static Processor* create(const std::string& type);

public: // abstract
    virtual ~Processor() = default;
    virtual bool isRunning() const = 0;
    virtual bool init(const Settings& settings) = 0;

protected: // vars
    Settings _settings;

};