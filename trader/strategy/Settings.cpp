#include "Settings.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "exchanger/Exchanger.hpp"

NS_USE

static const char* MODE = "MODE";
static const char* USERNAME = "REDIS_USERNAME";
static const char* SYMBOL = "SYMBOL";
static const char* LOT_SIZE = "LOT_SIZE";
static const char* TAKE_PROFIT = "TAKE_PROFIT";
static const char* STOP_LOSS = "STOP_LOSS";
static const char* AVERAGING = "AVERAGING";
static const char* PROFIT_RATIO = "PROFIT_RATIO";

Settings::Settings(const core::Config& config) {
    mode = config.asString(MODE);
    username = config.asString(USERNAME);
    symbol = config.asString(SYMBOL);
    lot_size = config.asDouble(LOT_SIZE);
    take_profit = config.asDouble(TAKE_PROFIT) / 100.0;
    stop_loss = config.asDouble(STOP_LOSS) / -100.0;
    averaging = config.asDouble(AVERAGING) / -100.0;
    profit_ratio = config.asDouble(PROFIT_RATIO);
}

bool Settings::isValid() const {
    if (not isRelease() && not isDevelop() && not isBackTest()) {
        Logger::info("Settings: unknown %s(%s)", MODE, mode.c_str());
        return false;
    }

    if (Exchanger().pair(symbol) == nullptr) {
        Logger::info("Settings: %s(%s) doesn't exist", SYMBOL, symbol.c_str());
        return false;
    }

    if (lot_size < 1.0) {
        Logger::info("Settings: %s < 1.0", LOT_SIZE);
        return false;
    }

    if (profit_ratio < 0.0 || profit_ratio > 1.0) {
        Logger::info("Settings: %s must be between 0 to 1", PROFIT_RATIO);
        return false;
    }

    return true;
}

bool Settings::isRelease() const {
    return mode == "release";
}

bool Settings::isDevelop() const {
    return mode == "develop";
}

bool Settings::isBackTest() const {
    return mode == "backtest";
}

std::string Settings::uniqId() const {
    return username + ":" + symbol.id();
}