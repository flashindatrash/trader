#pragma once

#include "Config.hpp"

namespace core {

class App {
protected: // methods
    App(const Config& config);
    virtual ~App() = default;

    void sleep_ms(int ms);

protected: // vars
    Config _config;
};

}
