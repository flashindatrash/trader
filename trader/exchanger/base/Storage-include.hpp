#pragma once

#include "exchanger/base/Storage.hpp"
#include "exchanger/base/Storage-macros.hpp"
#include "exchanger/wrapper/ExchangeWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"

class Storage {
    MAP(std::string, ExchangeWrapper, info)
    MAP(std::string, PriceWrapper, price)
    MAP(std::string, BalanceWrapper, balance)
    MAP(std::string, CandlestickWrapper, daily_change)
    MAP(std::string, ChartWrapper, chart)
};
