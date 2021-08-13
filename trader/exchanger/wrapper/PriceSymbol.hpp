#pragma once

#include <global.hpp>
#include "exchanger/wrapper/PriceBase.hpp"
#include "exchanger/binance/response/BinancePriceStatisticsData.hpp"

class PriceSymbol
{
public: // static
    typedef std::pair<Price, time_t> PriceTimePair;

    static PriceSymbol* create();

public: // methods
    void add(Price price);
    void add(Price price, time_t time);

    double getCurrent() const;

    double getPriceBack(time_t interval) const;
    double getPriceAverage(time_t interval) const;

    BinancePriceStatisticsData& getStats();

protected: // methods
    PriceSymbol() = default;

protected: // vars
    BinancePriceStatisticsData _per_day;
    std::vector<PriceTimePair> _per_second;
};

