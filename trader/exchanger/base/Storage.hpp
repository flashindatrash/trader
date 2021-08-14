#pragma once

#include "exchanger/base/Storage-macros.hpp"
#include "exchanger/wrapper/ExchangeWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"

class Storage {
    MAP(std::string, ExchangeWrapper, info)
    MAP(std::string, PriceWrapper, price)
    MAP(std::string, BalanceWrapper, balance)
};
