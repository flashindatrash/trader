#pragma once

#include "Defines.hpp"

namespace core {
    class Config;
}

NS_BEGIN
struct Settings {
public: // methods
    explicit Settings(const core::Config& config);

    std::string uniqId() const;
    bool isValid() const;

protected: // methods
    Settings() = default;

public: // vars
    bool test = false;

    std::string username;

    Symbol symbol;

    double take_profit = 0.0;
    double stop_loss = 0.0;

    double lot_min = 1.0;
    double lot_max = 1.0;

    double risk = 0.5;
};
NS_END
