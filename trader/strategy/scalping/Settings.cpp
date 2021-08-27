#include "Settings.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "exchanger/Exchanger.hpp"

NS_USE

Settings::Settings(const core::Config& config)
{
    symbol = config.asString("SYMBOL");
    test = config.asBool("TEST");
    open_next_price_percent = config.asDouble("OPEN_NEXT_PRICE_PERCENT") / 100.0;
    open_next_lot_multiply = config.asDouble("OPEN_NEXT_LOT_MULTIPLY");
    close_position_percent = config.asDouble("CLOSE_POSITION_PERCENT") / 100.0;
    balance_base_limit = config.asDouble("BALANCE_BASE_LIMIT");
    balance_quote_limit = config.asDouble("BALANCE_QUOTE_LIMIT");
}

bool Settings::isValid() const {
    if (Exchanger().pair(symbol) == nullptr) {
        Logger::info("%s symbol doesn't exist", symbol.c_str());
        return false;
    }

    if (open_next_price_percent < 0.0) {
        Logger::info("Invalid OPEN_NEXT_PRICE_PERCENT settings (%f)", open_next_price_percent);
        return false;
    }

    if (open_next_lot_multiply < 1.0) {
        Logger::info("Invalid OPEN_NEXT_LOT_MULTIPLY settings (%f)", open_next_lot_multiply);
        return false;
    }

    if (close_position_percent < 0.0) {
        Logger::info("Invalid CLOSE_POSITION_PERCENT settings (%f)", close_position_percent);
        return false;
    }

    return true;
}
