#include <argparser/ArgumentParser.hpp>

#include "get_price.hpp"
#include "get_symbolinfo.hpp"

int main(int argc, char** argv) {
    srand(time(NULL));

    cppargparser::ArgumentParser args;
    args.addArgument(cppargparser::Argument("-a", "--action", "price", 1, true));
    args.addArgument(cppargparser::Argument("-s", "--symbol", "symbol", 1, false));

    auto parsed = args.parse(argc, argv);
    std::string action = parsed.getValue("--action");
    std::string symbol = parsed.getValue("--symbol");

    if (action == "price" && not symbol.empty())
        return Tools::get_price(symbol);
    if (action == "symbolinfo" && not symbol.empty())
        return Tools::get_symbolinfo(symbol);

    return EXIT_FAILURE;
}
