#include "PriceAnalyzer.hpp"

PriceAnalyzer::PriceAnalyzer(const KlineHistory& history)
    : _history(history)
{
}

double PriceAnalyzer::predictNextPrice() const {
    return 0.0;
}

