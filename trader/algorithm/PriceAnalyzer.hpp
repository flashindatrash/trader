#pragma once

#include <global.hpp>

class KlineHistory;

class PriceAnalyzer
{
public: // methods
    PriceAnalyzer(const KlineHistory& history);

    double getChangeSince(time_t time) const;

protected: // vars
    const KlineHistory& _history;
};

