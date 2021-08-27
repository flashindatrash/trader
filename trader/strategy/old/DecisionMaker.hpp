#pragma once

#include <vector>

struct OrderRequest;
class OrderWrapper;

namespace scalping {

class DecisionMaker {
public: // struct
    enum BasedOn {
        Balance = 1,
        DayChange = 2,

        ForTrader = Balance | DayChange,
        ForProfit = DayChange
    };

public: // methods
    DecisionMaker(const std::vector<const OrderWrapper*>& positions);

    double factor(const OrderRequest& request, int based_on) const;

protected: // methods
    bool has(int mask, BasedOn value) const;

protected: // vars
    const std::vector<const OrderWrapper*>& _positions;
};

}

