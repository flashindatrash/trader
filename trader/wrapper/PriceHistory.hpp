#pragma once

#include <global.hpp>

class PriceHistory
{
    typedef std::pair<double, time_t> Data;

public: // static
    static PriceHistory* create();

public: // methods
    void add(double price);
    void add(double price, time_t time);

    double getPriceBack(time_t interval) const;
    double getPriceAverage(time_t interval) const;
    float getPriceChangePercent(time_t interval) const;

protected: // methods
    PriceHistory() {}

    std::vector<Data>& getStorage(time_t interval);

protected: // vars
    std::vector<Data> _per_second;
};

