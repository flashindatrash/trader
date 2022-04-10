#include "Settings.hpp"
#include "argparser/ArgumentParser.hpp"
#include "core/Logger.hpp"

bool Settings::parse(int argc, char** argv) {
    std::string config_file;

    cppargparser::ArgumentParser args;
    try {
        args.addArgument(cppargparser::Argument("-c", "--config", "config file", 1, true));
        args.addArgument(cppargparser::Argument("-t", "--type", "type", 1, true));
        args.addArgument(cppargparser::Argument("-l", "--lua", "lua file", 1, false));
        args.addArgument(cppargparser::Argument("-s", "--symbol", "symbol", 2, false));
        args.addArgument(cppargparser::Argument("-m", "--mode", "develop/release", 1, false));

        auto parsed = args.parse(argc, argv);

        config_file = parsed.getValue("--config");

        _type = parsed.getValue("--type");

        if (parsed.hasArgument("--lua"))
            _script = parsed.getValue("--lua");

        if (parsed.hasArgument("--symbol"))
            _symbol = Symbol(parsed.getValues("--symbol").at(0), parsed.getValues("--symbol").at(1));

        if (parsed.hasArgument("--mode"))
            _mode = parsed.getValue("--mode");

    } catch(...) {
        args.showHelp("trader -t trader -c default.cfg -l script.lua -s btc usdt | trader -t listing -c default.cfg");
        return false;
    }

    if (not _config.read(config_file.c_str())) {
        Logger::info(util::format("Can't load config %s", config_file.c_str()));
        return false;
    }

    return true;
}

const core::Config& Settings::config() const {
    return _config;
}

const std::string& Settings::type() const {
    return _type;
}

const std::string& Settings::mode() const {
    return _mode;
}

const std::string& Settings::script() const {
    return _script;
}

const std::string& Settings::username() const {
    static const char* USERNAME = "REDIS_USERNAME";
    return _config.asString(USERNAME);
}

const Symbol& Settings::symbol() const {
    return _symbol;
}

double Settings::profitRatio() const {
    return _profit_ratio;
}

bool Settings::isRelease() const {
    return _mode == "release";
}

bool Settings::isBackTest() const {
    return _mode == "backtest";
}

bool Settings::isDevelop() const {
    return not isRelease() && not isBackTest();
}

bool Settings::logEnabled() const {
    return true || not isBackTest();
}