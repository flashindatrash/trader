#pragma once

#include "exchanger/base/Symbol.hpp"

namespace core {
    class Config;
}

namespace pair {
struct Settings {
public: // methods
    Settings() = default;
    explicit Settings(const core::Config& config);

    std::string storage(const std::string& key) const;

    bool isValid() const;
    bool isRelease() const;
    bool isDevelop() const;
    bool isBackTest() const;

    bool logEnabled() const;

public: // vars
    std::string mode;
    std::string username;
    std::string script;

    Symbol symbol;

    double profit_ratio = 0.0;
};
}
