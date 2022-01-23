#pragma once

#include "core/App.hpp"
#include "base/Settings.hpp"

class BaseStrategy;

class TraderApp : public core::App {
public: // static
    static TraderApp* create(const Settings& settings);

public: // methods
    int run();

protected: // methods
    explicit TraderApp();
    ~TraderApp() override;

    bool init(const Settings& settings);

protected: // vars
    Settings _settings;
    BaseStrategy* _strategy = nullptr;
};

