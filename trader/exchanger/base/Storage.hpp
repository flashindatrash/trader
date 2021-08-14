#pragma once

#include "exchanger/base/Storage-macros.hpp"
#include "exchanger/wrapper/SymbolInfo.hpp"
#include "exchanger/wrapper/PriceContainer.hpp"
#include "exchanger/wrapper/Balance.hpp"

class Storage {
    MAP(std::string, SymbolInfo, info)
    MAP(std::string, PriceContainer, price)
    MAP(std::string, Balance, balance)
};
