#pragma once

class Symbol;
class BinanceSideEnum;

class DecisionMaker
{
public: // struct
    enum BasedOn {
        Balance = 1,
        DayChange = 2,

        ForTrader = Balance | DayChange,
        ForProfit = DayChange
    };

public: // methods
    DecisionMaker(const Symbol& symbol);

    double factor(const BinanceSideEnum& side, int based_on) const;

protected: // methods
    bool has(int mask, BasedOn value) const;

protected: // vars
    const Symbol& _symbol;

};

