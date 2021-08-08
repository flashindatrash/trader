#pragma once

class TradeSymbol;

class DecisionMaker
{
public: // struct
    enum BasedOn {
        Balane = 1
    };

public: // methods
    DecisionMaker(const TradeSymbol& symbol);

    bool make(double change, double min, double max, int based_on);

protected: // methods
    bool has(int mask, BasedOn value);

protected: // vars
    const TradeSymbol& _symbol;

};

