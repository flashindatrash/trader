#pragma once

#include "BaseApp.hpp"

class Strategy;

class TraderApp : public BaseApp {
public: // static
    static TraderApp* create(const core::Config& config);

public: // methods
    int run() override;

protected: // methods
    explicit TraderApp(const core::Config& config);
    ~TraderApp() override;

protected: // vars
    Strategy* _strategy = nullptr;
};

