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
    double open_next_price_percent = 0.0;
    double open_next_lot_multiply = 1.0;
    double close_position_percent = 0.0;

    double balance_base_limit = 0.0;
    double balance_quote_limit = 0.0;
};
NS_END
