#pragma once

#include <global.hpp>

class KlineHistory;

class PriceAnalyzer
{
public: // methods
    PriceAnalyzer(const KlineHistory& history);

    double getStablePriceChange(time_t since) const;

protected: // vars
    const KlineHistory& _history;
};

