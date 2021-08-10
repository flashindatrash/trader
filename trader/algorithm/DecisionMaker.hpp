#pragma once

class TradeSymbol;

class DecisionMaker
{
public: // struct
    enum BasedOn {
        Balance = 1,
        DayChange = 2,

        ForTrader = DayChange,
        ForProfit = DayChange
    };

public: // methods
    DecisionMaker(const TradeSymbol& symbol);

    double factor(double base, int based_on) const;
    bool make(double base, int based_on) const;

protected: // methods
    bool has(int mask, BasedOn value) const;

protected: // vars
    const TradeSymbol& _symbol;

};

