#pragma once

#include "exchanger/base/Storage.hpp"
#include "exchanger/base/Storage-macros.hpp"
#include "exchanger/wrapper/ExchangeWrapper.hpp"
#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/CandlestickWrapper.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/BookWrapper.hpp"

class Storage {
    STORAGE_PUBLIC(std::string, ExchangeWrapper, info)
    STORAGE_PUBLIC(std::string, PriceWrapper, price)
    STORAGE_PUBLIC(std::string, BalanceWrapper, balance)
    STORAGE_PRIVATE(std::string, CandlestickWrapper, stat)
    STORAGE_PRIVATE(std::string, ChartWrapper, chart)
    STORAGE_PRIVATE(std::string, BookWrapper, book)
};
