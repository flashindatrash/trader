#include <argparser/ArgumentParser.hpp>
#include "Config.hpp"
#include "Logger.hpp"
#include "get_price.hpp"
#include "get_symbolinfo.hpp"
#include "util/StringUtil.hpp"

int main(int argc, char** argv) {
    srand(time(NULL));

    cppargparser::ArgumentParser args;
    args.addArgument(cppargparser::Argument("-c", "--config", "config", 1, true));
    args.addArgument(cppargparser::Argument("-a", "--action", "action", 1, true));
    args.addArgument(cppargparser::Argument("-s", "--symbol", "symbol", 1, true));

    auto parsed = args.parse(argc, argv);
    std::string cfg_file = parsed.getValue("--config");
    std::string action = parsed.getValue("--action");
    std::string symbol = parsed.getValue("--symbol");

    core::Config cfg;
    if (!cfg.read(cfg_file.c_str())) {
        Logger::info(util::format("Can't init config %s", cfg_file.c_str()));
        return EXIT_FAILURE;
    }

    if (action == "price" && not symbol.empty())
        return tools::get_price(cfg, symbol);
    if (action == "symbolinfo" && not symbol.empty())
        return tools::get_symbolinfo(cfg, symbol);

    return EXIT_FAILURE;
}
