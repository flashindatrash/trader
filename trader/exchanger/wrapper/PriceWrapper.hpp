#pragma once

#include "exchanger/base/Storage.hpp"
#include "exchanger/base/ExchangerTypes.hpp"

class PriceWrapper : public MapIdentifier<std::string>
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

protected: // methods
    PriceWrapper() = default;

protected: // vars
    std::vector<PriceTimePair> _per_second;
};

