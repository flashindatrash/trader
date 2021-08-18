#pragma once

#include "App.hpp"
#include "Version.hpp"

class Strategy;

class TraderApp : public core::App {
public: // static
    static core::Version sVersion;

    // create app
    static TraderApp* create(const core::Config& config);

public: // methods
    // run app
    int run();

protected: // methods
    TraderApp(const core::Config& config);
    virtual ~TraderApp();

protected: // vars
    Strategy* _strategy = nullptr;
};

