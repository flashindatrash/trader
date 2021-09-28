#include "Settings.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "exchanger/Exchanger.hpp"

NS_USE

static const char* MODE = "MODE";
static const char* USERNAME = "REDIS_USERNAME";
static const char* SYMBOL = "SYMBOL";
static const char* LOT_MIN = "LOT_MIN";
static const char* LOT_MAX = "LOT_MAX";
static const char* TAKE_PROFIT = "TAKE_PROFIT";
static const char* STOP_LOSS = "STOP_LOSS";

Settings::Settings(const core::Config& config) {
    mode = config.asString(MODE);
    username = config.asString(USERNAME);
    symbol = config.asString(SYMBOL);
    lot_min = config.asDouble(LOT_MIN);
    lot_max = config.asDouble(LOT_MAX);
    take_profit = config.asDouble(TAKE_PROFIT) / 100.0;
    stop_loss = config.asDouble(STOP_LOSS) / -100.0;
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

    if (take_profit <= 0.0) {
        Logger::info("Settings: %s <= 0.0", TAKE_PROFIT);
        return false;
    }

    if (stop_loss >= 0.0) {
        Logger::info("Settings: %s <= 0.0", STOP_LOSS);
        return false;
    }

    if (lot_min < 1.0) {
        Logger::info("Settings: %s < 1.0", LOT_MIN);
        return false;
    }

    if (lot_max < 1.0) {
        Logger::info("Settings: %s < 1.0", LOT_MAX);
        return false;
    }

    if (lot_max < lot_min) {
        Logger::info("Settings: %s < %s", LOT_MAX, LOT_MIN);
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