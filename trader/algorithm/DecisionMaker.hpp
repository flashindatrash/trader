#pragma once

#include <vector>

class OrderWrapper;
class Symbol;
enum OrderSide : unsigned int;

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
    DecisionMaker(const Symbol& symbol, const std::vector<const OrderWrapper*>& positions);

    double factor(const OrderSide& side, int based_on) const;

protected: // methods
    bool has(int mask, BasedOn value) const;

protected: // vars
    const Symbol& _symbol;
    const std::vector<const OrderWrapper*>& _positions;
};

