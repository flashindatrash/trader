#include "get_symbolinfo.hpp"

#include "iostream"
#include "binacpp.h"
#include "binacpp_logger.h"
#include "binacpp_utils.h"
#include "Config.hpp"

int tools::get_symbolinfo(const core::Config& cfg, std::string symbol) {
    // init binance api
    static string api_key       = cfg.getAsString("BINANCE_API_KEY");
    static string secret_key    = cfg.getAsString("BINANCE_SECRET_KEY");
    BinaCPP::init( api_key , secret_key );
    // init binance logger
    BinaCPP_logger::set_debug_level(0);

    Json::Value result;
    BinaCPP::get_exchangeInfo(result);

    const Json::Value& symbols = result["symbols"];
    if (not symbols.isArray()) {
        std::cout << "invalid response" << std::endl;
        return EXIT_FAILURE;
    }

    string_toupper(symbol);
    for (uint i = 0; i < symbols.size(); ++i) {
        const Json::Value& json = symbols[i];
        if (json["symbol"].asString() == symbol) {
            std::cout << json.toStyledString() << std::endl;
            return EXIT_SUCCESS;
        }
    }

    std::cout << "not found symbol" << std::endl;
    return EXIT_FAILURE;
}
