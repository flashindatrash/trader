#pragma once

#include "Defines.hpp"

namespace core {
    class Config;
}

NS_BEGIN
struct Settings {
public: // methods
    explicit Settings(const core::Config& config);

    std::string storage(const std::string& key) const;

    bool isValid() const;
    bool isRelease() const;
    bool isDevelop() const;
    bool isBackTest() const;
    bool isBalanceUnlimited() const;

protected: // methods
    Settings() = default;

public: // vars
    std::string mode;
    std::string username;

    Symbol symbol;

    double take_profit = 0.0;
    double stop_loss = 0.0;
    double averaging = 0.0;
    double lot_size = 1.0;
    double profit_ratio = 0.0;
    int open_filter = 0;
};
NS_END
