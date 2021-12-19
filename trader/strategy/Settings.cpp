#include "Settings.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "Storage.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/OrderBase.hpp"
#include "util/StringUtil.hpp"

NS_USE

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

bool Settings::isValid() const {
    if (not isRelease() && not isDevelop() && not isBackTest()) {
        Logger::info(util::format("Settings: unknown %s(%s)", MODE, mode.c_str()));
        return false;
    }

    if (script.empty()) {
        Logger::info(util::format("Settings: unknown %s", SCRIPT));
        return false;
    }

    if (Exchanger().pair(symbol) == nullptr) {
        Logger::info(util::format("Settings: %s(%s) doesn't exist", SYMBOL, symbol.c_str()));
        return false;
    }

    if (profit_ratio < 0.0 || profit_ratio > 1.0) {
        Logger::info(util::format("Settings: %s must be between 0 to 1", PROFIT_RATIO));
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

bool Settings::logEnabled() const {
    return true || not isBackTest();
}

std::string Settings::storage(const std::string& key) const {
    return protocol::Storage::key(username, symbol.id(), key);
}