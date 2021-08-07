#pragma once

class KlineHistory;

class PriceAnalyzer
{
public: // methods
    PriceAnalyzer(const KlineHistory& klines);

protected: // vars
    const KlineHistory& _klines;
};

