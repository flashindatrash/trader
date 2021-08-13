#pragma once

class Symbol;
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
    DecisionMaker(const Symbol& symbol);

    bool make(double rate, int based_on, double& factor = sDefaultRef) const;

protected: // methods
    double calc(const BinanceSideEnum& side, int based_on) const;
    bool has(int mask, BasedOn value) const;

protected: // vars
    const Symbol& _symbol;

};

