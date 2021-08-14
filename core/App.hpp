#pragma once

#include "Config.hpp"

namespace core {

class App {
public: // methods
    App(Config config);

protected: // methods
    void sleep_ms(int ms);

protected: // vars
    Config _config;
};

}
