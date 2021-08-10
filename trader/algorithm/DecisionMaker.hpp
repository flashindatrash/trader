#pragma once

#include "wrapper/PriceBase.hpp"

class TradeSymbol;

class DecisionMaker
{
private: // static
    static double sDefaultRef;

public: // struct
    enum BasedOn {
        Balance = 1,
        DayChange = 2,

        ForTrader = Balance | DayChange,
        ForProfit = DayChange
    };

public: // methods
    DecisionMaker(const TradeSymbol& symbol);

    bool make(Change base, int based_on, double& out = sDefaultRef) const;

protected: // methods
    double factor(double base, int based_on) const;

    bool has(int mask, BasedOn value) const;

protected: // vars
    const TradeSymbol& _symbol;

};

