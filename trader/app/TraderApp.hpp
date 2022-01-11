#pragma once

#include "core/App.hpp"

class BaseStrategy;

class TraderApp : public core::App {
public: // static
    static TraderApp* create(const std::string& type, const core::Config& config);

public: // methods
    int run();

protected: // methods
    explicit TraderApp(const core::Config& config);
    ~TraderApp() override;

    bool init(const std::string& type);

protected: // vars
    BaseStrategy* _strategy = nullptr;
};

