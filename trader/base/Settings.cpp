#include "Settings.hpp"
#include "core/Config.hpp"

static const char* MODE = "MODE";
static const char* USERNAME = "REDIS_USERNAME";
static const char* SCRIPT = "SCRIPT";
static const char* SYMBOL = "SYMBOL";
static const char* PROFIT_RATIO = "PROFIT_RATIO";

Settings::Settings(const core::Config& config) {
    mode = config.asString(MODE);
    username = config.asString(USERNAME);
    script = config.asString(SCRIPT);
    symbol = config.asString(SYMBOL);
    profit_ratio = config.asDouble(PROFIT_RATIO);
}

bool Settings::isRelease() const {
    return mode == "release";
}

bool Settings::isBackTest() const {
    return mode == "backtest";
}

bool Settings::isDevelop() const {
    return not isRelease() && not isBackTest();
}

bool Settings::logEnabled() const {
    return true || not isBackTest();
}