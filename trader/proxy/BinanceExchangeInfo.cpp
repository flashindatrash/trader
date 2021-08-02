#include "BinanceExchangeInfo.hpp"

#include "binacpp.h"

BinanceExchangeInfo::BinanceExchangeInfo() {
}

bool BinanceExchangeInfo::init() {
    Json::Value result;
    BinaCPP::get_exchangeInfo(result);
    return true;
}
