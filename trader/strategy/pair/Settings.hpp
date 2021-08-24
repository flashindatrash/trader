#pragma once

#include "Defines.hpp"

namespace core {
    class Config;
}

NS_BEGIN
struct Settings {
public: // methods
    Settings(const core::Config& config);

    bool isValid() const;

protected: // methods
    Settings() = default;

public: // vars
    Symbol symbol;
    bool test;
};
NS_END
