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
}

bool Settings::isValid() const {
    if (Exchanger().pair(symbol) == nullptr) {
        Logger::info("%s symbol doesn't exist", symbol.c_str());
        return false;
    }

    if (open_next_lot_multiply < 1.0) {
        Logger::info("Invalid OPEN_NEXT_LOT_MULTIPLY parameter (%f)", open_next_lot_multiply);
        return false;
    }

    return true;
}
