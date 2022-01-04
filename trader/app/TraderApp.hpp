#pragma once

#include "App.hpp"

class Strategy;

class TraderApp : public core::App {
public: // static
    static TraderApp* create(const core::Config& config);

public: // methods
    int run();

protected: // methods
    explicit TraderApp(const core::Config& config);
    ~TraderApp() override;

protected: // vars
    Strategy* _strategy = nullptr;
};

