#pragma once

#include <string>

namespace core {
    class Config;
}

struct Settings {
public: // methods
    Settings() = default;
    explicit Settings(const core::Config& config);

    bool isRelease() const;
    bool isBackTest() const;
    bool isDevelop() const;

    bool logEnabled() const;

public: // vars
    std::string mode;
    std::string username;
    std::string script;
    std::string symbol;
    double profit_ratio = 0.0;
};
