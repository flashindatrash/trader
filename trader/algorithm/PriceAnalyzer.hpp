#pragma once

#include <global.hpp>
#include "exchanger/wrapper/PriceWrapper.hpp"

class CandlestickContainer;

class PriceAnalyzer
{
public: // methods
    PriceAnalyzer(const CandlestickContainer& history);

    Change getStablePriceChange(time_t since) const;

protected: // vars
    const CandlestickContainer& _history;
};

