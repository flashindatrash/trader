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
static const char* SYMBOL = "SYMBOL";
static const char* LOT_SIZE = "LOT_SIZE";
static const char* TAKE_PROFIT = "TAKE_PROFIT";
static const char* STOP_LOSS = "STOP_LOSS";
static const char* AVERAGING = "AVERAGING";
static const char* PROFIT_RATIO = "PROFIT_RATIO";
static const char* OPEN_FILTER = "OPEN_FILTER";

Settings::Settings(const core::Config& config) {
    mode = config.asString(MODE);
    username = config.asString(USERNAME);
    symbol = config.asString(SYMBOL);
    lot_size = config.asDouble(LOT_SIZE);
    take_profit = config.asDouble(TAKE_PROFIT) / 100.0;
    stop_loss = config.asDouble(STOP_LOSS) / -100.0;
    averaging = config.asDouble(AVERAGING) / -100.0;
    profit_ratio = config.asDouble(PROFIT_RATIO);
    open_filter = config.asInt(OPEN_FILTER);
}

bool Settings::isValid() const {
    if (not isRelease() && not isDevelop() && not isBackTest()) {
        Logger::info(util::format("Settings: unknown %s(%s)", MODE, mode.c_str()));
        return false;
    }

    if (Exchanger().pair(symbol) == nullptr) {
        Logger::info(util::format("Settings: %s(%s) doesn't exist", SYMBOL, symbol.c_str()));
        return false;
    }

    if (lot_size < 1.0) {
        Logger::info(util::format("Settings: %s < 1.0", LOT_SIZE));
        return false;
    }

    if (profit_ratio < 0.0 || profit_ratio > 1.0) {
        Logger::info(util::format("Settings: %s must be between 0 to 1", PROFIT_RATIO));
        return false;
    }

    if (open_filter != OrderSide::Invalid && open_filter != OrderSide::Buy && open_filter != OrderSide::Sell) {
        Logger::info(util::format("Settings: invalid %s param %d", OPEN_FILTER, open_filter));
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

bool Settings::isBalanceUnlimited() const {
    return isBackTest() && false;
}

std::string Settings::storage(const std::string& key) const {
    return protocol::Storage::key(username, symbol.id(), key);
}