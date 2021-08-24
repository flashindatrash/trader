#include "Settings.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "exchanger/Exchanger.hpp"

NS_USE

Settings::Settings(const core::Config& config)
{
    symbol = config.getAsString("SYMBOL");
    test = config.getAsBool("TEST");
}

bool Settings::isValid() const {
    if (Exchanger().pair(symbol) == nullptr) {
        Logger::info("%s symbol doesn't exist", symbol.c_str());
        return false;
    }
    return true;
}
