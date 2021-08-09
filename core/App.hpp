#pragma once

#include "Config.hpp"

namespace core {

class App
{
public: // methods
    App(Config config);

protected: // vars
    Config _config;
};

}
