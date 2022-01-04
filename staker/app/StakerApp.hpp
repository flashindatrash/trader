//
// Created by Вадим Проскурин on 04.01.2022.
//

#pragma once

#include "App.hpp"

class StakerApp : public core::App {
public: // static
    static StakerApp* create(const core::Config& config);

public: // methods
    int run();

protected: // methods
    explicit StakerApp(const core::Config& config);
    ~StakerApp() override = default;

protected: // vars
};


