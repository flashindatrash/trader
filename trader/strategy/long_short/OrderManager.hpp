#pragma once

#include <vector>
#include <string>

class Symbol;
class OrderWrapper;
struct OrderRequest;

class OrderManager {
public: // methods
    OrderManager(const Symbol& symbol, bool test_mode);

    bool create(const OrderRequest& request, const OrderWrapper* position);

    const std::vector<const OrderWrapper*>& getPositions() const;

protected: // methods
    void sortPositions();
    void printOrder(const OrderWrapper* order, const OrderWrapper* position = nullptr);

protected: // vars
    const bool _test_mode;
    std::vector<const OrderWrapper*> _positions;
};

