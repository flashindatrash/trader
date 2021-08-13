#pragma once

#include "Proxy.hpp"

class Symbol;
struct BinancePriceStatisticsData;

class BinancePrices : public core::Proxy<BinancePrices>
{
public: // methods
    BinancePrices() = default;

    // get 24hr statistics
    const BinancePriceStatisticsData& getStats(const Symbol& symbol);
};

#define SPrices() BinancePrices::getInstance()
