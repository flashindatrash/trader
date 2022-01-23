#pragma once

#include "core/Config.hpp"
#include "exchanger/base/Symbol.hpp"
#include <string>

class Settings {
public: // methods
    Settings() = default;

    bool parse(int argc, char** argv);

    const core::Config& config() const;
    const std::string& type() const;
    const std::string& mode() const;
    const std::string& script() const;
    const std::string& username() const;
    const Symbol& symbol() const;
    double profitRatio() const;

    bool isRelease() const;
    bool isBackTest() const;
    bool isDevelop() const;

    bool logEnabled() const;

protected: // vars
    core::Config _config;
    std::string _type;
    std::string _mode;
    std::string _script;
    Symbol _symbol;
    double _profit_ratio = 0.0;
};
