#include "Settings.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "exchanger/Exchanger.hpp"

NS_USE

Settings::Settings(const core::Config& config) {
    username = config.asString("REDIS_USERNAME");
    symbol = config.asString("SYMBOL");
    test = config.asBool("TEST");
    open_price_percent = config.asDouble("OPEN_PRICE_PERCENT") / 100.0;
    open_lot_multiply = config.asDouble("OPEN_LOT_MULTIPLY");
    open_max_multiply = config.asDouble("OPEN_MAX_MULTIPLY");
    close_position_percent = config.asDouble("CLOSE_POSITION_PERCENT") / 100.0;
    balance_base_limit = config.asDouble("BALANCE_BASE_LIMIT");
    balance_quote_limit = config.asDouble("BALANCE_QUOTE_LIMIT");
    strong_tail_percent = config.asDouble("STRONG_TAIL_PERCENT") / 100.0;
}

bool Settings::isValid() const {
    if (Exchanger().pair(symbol) == nullptr) {
        Logger::info("%s symbol doesn't exist", symbol.c_str());
        return false;
    }

    if (open_price_percent < 0.0) {
        Logger::info("Invalid OPEN_PRICE_PERCENT settings (%f)", open_price_percent);
        return false;
    }

    if (open_lot_multiply < 1.0) {
        Logger::info("Invalid OPEN_LOT_MULTIPLY settings (%f)", open_lot_multiply);
        return false;
    }

    if (close_position_percent < 0.0) {
        Logger::info("Invalid CLOSE_POSITION_PERCENT settings (%f)", close_position_percent);
        return false;
    }

    return true;
}

std::string Settings::uniqId() const {
    return username + ":" + symbol.id();
}