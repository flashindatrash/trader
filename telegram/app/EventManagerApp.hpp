//
// Created by Вадим Проскурин on 01.11.2021.
//

#pragma once

#include "core/App.hpp"

class EventManagerApp : public core::App {
public: // static
    static EventManagerApp* create(const core::Config& config);

public: // methods
    int run();

protected: // methods
    EventManagerApp(const core::Config& config);
};


