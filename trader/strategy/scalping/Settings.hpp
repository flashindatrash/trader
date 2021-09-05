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
    std::string username;

    Symbol symbol;
    bool test = false;
    double price_distance = 0.0;
    double open_lot_multiply = 1.0;
    double open_max_multiply = 1.0;
    double close_position_percent = 0.0;
    double strong_tail_percent = 0.0;
};
NS_END
