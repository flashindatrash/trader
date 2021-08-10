#pragma once

#include <global.hpp>
#include "wrapper/PriceBase.hpp"

class KlineHistory;

class PriceAnalyzer
{
public: // methods
    PriceAnalyzer(const KlineHistory& history);

    Change getStablePriceChange(time_t since) const;

protected: // vars
    const KlineHistory& _history;
};

