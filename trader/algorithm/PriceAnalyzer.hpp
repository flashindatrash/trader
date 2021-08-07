#pragma once

class KlineHistory;

class PriceAnalyzer
{
public: // methods
    PriceAnalyzer(const KlineHistory& history);

    double predictNextPrice() const;

protected: // vars
    const KlineHistory& _history;
};

