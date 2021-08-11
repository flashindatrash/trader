#pragma once

#include "wrapper/PriceBase.hpp"

class TradeSymbol;
class BinanceSideEnum;

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

    bool make(Change change, int based_on, double& factor = sDefaultRef) const;

protected: // methods
    double calc(const BinanceSideEnum& side, int based_on) const;
    bool has(int mask, BasedOn value) const;

protected: // vars
    const TradeSymbol& _symbol;

};

