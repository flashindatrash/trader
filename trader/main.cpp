#include <argparser/ArgumentParser.hpp>
#include "Logger.hpp"
#include "TraderApp.hpp"
#include "wrapper/TradeSymbol.hpp"

int main(int argc, char** argv) {
    srand(time(NULL));

    TradeSymbol symbol;
    cppargparser::ArgumentParser args;
    try {
        args.addArgument(cppargparser::Argument("-s", "--symbol", "symbol", 2, true));

        auto parsed = args.parse(argc, argv);
        std::vector<std::string> assets = parsed.getValues("--symbol");
        symbol = TradeSymbol(assets[0], assets[1]);
    } catch(...) {
        args.showHelp("trader -s btc usdt");
        return EXIT_FAILURE;
    }

    #ifdef NDEBUG
    try {
        TraderApp::create()->run(symbol);
    } catch (const std::exception& e) {
        trace("TraderApp failed with %s\n", e.what());
        return EXIT_FAILURE;
    }
    #else
    TraderApp::create()->run(symbol);
    #endif

	return EXIT_SUCCESS;
}
