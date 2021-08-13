#pragma once

#include "App.hpp"
#include "Version.hpp"

class Symbol;

class TraderApp : public core::App
{
public: // static
    static core::Version sVersion;

    // create app
    static TraderApp* create(core::Config config);

public: // methods
    // run app
    void run(const Symbol& symbol);

protected: // methods
    TraderApp(core::Config config);

};

