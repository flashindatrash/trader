#include <argparser/ArgumentParser.hpp>
#include "Logger.hpp"
#include "Config.hpp"
#include "TraderApp.hpp"
#include "wrapper/TradeSymbol.hpp"

int main(int argc, char** argv) {
    srand(time(NULL));

    TradeSymbol symbol;
    std::string cfg_file;

    cppargparser::ArgumentParser args;
    try {
        args.addArgument(cppargparser::Argument("-c", "--config", "config", 1, true));
        args.addArgument(cppargparser::Argument("-s", "--symbol", "symbol", 2, true));

        auto parsed = args.parse(argc, argv);

        cfg_file = parsed.getValue("--config");
        std::vector<std::string> assets = parsed.getValues("--symbol");
        symbol = TradeSymbol(assets[0], assets[1]);
    } catch(...) {
        args.showHelp("trader -c ./config/default.cfg -s btc usdt");
        return EXIT_FAILURE;
    }

    core::Config cfg;
    if (!cfg.read(cfg_file.c_str())) {
        trace("Can't init config %s", cfg_file.c_str());
        return EXIT_FAILURE;
    }

    #ifdef NDEBUG
    try {
        TraderApp::create(cfg)->run(symbol);
    } catch (const std::exception& e) {
        trace("TraderApp failed with %s\n", e.what());
        return EXIT_FAILURE;
    }
    #else
    TraderApp::create(cfg)->run(symbol);
    #endif

	return EXIT_SUCCESS;
}
