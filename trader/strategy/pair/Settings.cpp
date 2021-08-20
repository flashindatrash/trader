#include "Settings.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "proxy/Exchanger.hpp"

NS_USE

Settings::Settings(const core::Config& config)
{
    pair = config.getAsString("PAIR");
    quote = config.getAsString("QUOTE");
    test = config.getAsBool("TEST");
}

bool Settings::isValid() const {
    if (Exchanger().pair(pair) == nullptr) {
        Logger::info("%s pair doesn't exist", pair.c_str());
        return false;
    }

    if (Exchanger().pair(pair.baseAsset() + quote) == nullptr) {
        Logger::info("%s pair doesn't exist", (pair.baseAsset() + quote) .c_str());
        return false;
    }

    if (Exchanger().pair(pair.quoteAsset() + quote) == nullptr) {
        Logger::info("%s pair doesn't exist", (pair.quoteAsset() + quote).c_str());
        return false;
    }

    return true;
}
