#pragma once

#include "Config.hpp"
#include "Version.hpp"

namespace core {

class App {
protected: // methods
    explicit App(Config config);
    App(Config config, Version version);
    virtual ~App() = default;

    void sleep_ms(int ms);

protected: // vars
    Config _config;
    Version _version;
};

}
