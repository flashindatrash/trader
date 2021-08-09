#include "get_price.hpp"

#include "iostream"
#include "binacpp.h"
#include "binacpp_logger.h"
#include "Config.hpp"

int tools::get_price(const core::Config& cfg, std::string symbol) {
    // init binance api
    static string api_key       = cfg.getAsString("BINANCE_API_KEY");
    static string secret_key    = cfg.getAsString("BINANCE_SECRET_KEY");
    BinaCPP::init( api_key , secret_key );
    // init binance logger
    BinaCPP_logger::set_debug_level(0);

    Json::Value result;
    BinaCPP::get_avgPrice(symbol.c_str(), result);

    std::cout << result["price"].asString() << std::endl;
    return EXIT_SUCCESS;
}
