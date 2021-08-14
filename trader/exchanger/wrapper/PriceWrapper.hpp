#pragma once

#include <global.hpp>
#include "exchanger/base/ExchangerTypes.hpp"
#include "exchanger/binance/response/BinancePriceStatisticsData.hpp"

class PriceWrapper
{
public: // static
    typedef std::pair<Price, time_t> PriceTimePair;

    static PriceWrapper* create();

public: // methods
    void add(Price price);
    void add(Price price, time_t time);

    double get() const;

    double getPriceBack(time_t interval) const;
    double getPriceAverage(time_t interval) const;

    BinancePriceStatisticsData& getStats();

protected: // methods
    PriceWrapper() = default;

protected: // vars
    BinancePriceStatisticsData _per_day;
    std::vector<PriceTimePair> _per_second;
};

