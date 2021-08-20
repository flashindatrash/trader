#pragma once

#include <vector>
#include "exchanger/base/Identifier.hpp"
#include "exchanger/base/BaseTypes.hpp"

class PriceWrapper : public Identifier {
public: // static
    typedef std::pair<Price, time_t> PriceTimePair;

    static PriceWrapper* create();

public: // methods
    void add(Price price);
    void add(Price price, time_t time);

    const Price& get() const;

    const Price getPriceBack(time_t interval) const;
    const Price getPriceAverage(time_t interval) const;

protected: // methods
    PriceWrapper() = default;

protected: // vars
    std::vector<PriceTimePair> _per_second;
};

