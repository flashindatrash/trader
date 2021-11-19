//
// Created by Вадим Проскурин on 06.11.2021.
//

#pragma once

#include "Defines.hpp"
#include "Settings.hpp"

NS_BEGIN
class Algorithm;
class Context;
class Listener;

class Reactor {
public: // static
    static Reactor* create(const Settings& settings);

public: // methods
    bool init();

    void process(Algorithm& algorithm, const Context& context, const Listener& listener) const;

protected: // methods
    explicit Reactor(Settings settings);

protected: // vars
    const Settings _settings;

};
NS_END