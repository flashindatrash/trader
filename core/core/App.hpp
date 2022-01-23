#pragma once

#include "Version.hpp"

namespace core {

class App {
protected: // methods
    App() = default;
    explicit App(Version version);
    virtual ~App() = default;

    void sleep_ms(int ms);

protected: // vars
    Version _version;
};

}
