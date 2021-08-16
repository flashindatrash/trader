#pragma once

#include <vector>
#include <string>

class Symbol;
class OrderWrapper;
struct OrderRequest;

class OrderManager
{
protected: // static
    static const std::string key(const OrderWrapper& transaction);

public: // methods
    OrderManager(const Symbol& symbol, bool test_mode);

    bool create(const OrderRequest& request, const OrderWrapper* transaction);

    const std::vector<const OrderWrapper*>& getPositions() const;

protected: // methods
    void printProfit(const Symbol& symbol, double profit);

protected: // vars
    const bool _test_mode;

    std::vector<const OrderWrapper*> _positions;
};

