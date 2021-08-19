#pragma once

#include <vector>
#include <string>

class Symbol;
class OrderWrapper;
struct OrderRequest;

namespace scalping {

class OrderManager {
public: // methods
    OrderManager(const Symbol& symbol);

    bool create(const OrderRequest& request, const OrderWrapper* position);

    const std::vector<const OrderWrapper*>& getPositions() const;

protected: // methods
    void sortPositions();
    void printOrder(const Symbol& symbol, const OrderWrapper* order, const OrderWrapper* position = nullptr);

protected: // vars
    std::vector<const OrderWrapper*> _positions;
};

}
