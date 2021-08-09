#pragma once

#include <global.hpp>
#include "data/BinancePriceStatisticsData.hpp"

class PriceSymbol
{
public: // static
    typedef std::pair<double, time_t> PriceTimePair;

    static PriceSymbol* create();

public: // methods
    void add(double price);
    void add(double price, time_t time);

    double getCurrent() const;

    double getPriceBack(time_t interval) const;
    double getPriceAverage(time_t interval) const;

    BinancePriceStatisticsData& getStats();

protected: // methods
    PriceSymbol() {}

protected: // vars
    BinancePriceStatisticsData _per_day;
    std::vector<PriceTimePair> _per_second;
};

