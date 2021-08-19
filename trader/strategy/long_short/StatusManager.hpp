#pragma once

#include "BaseManager.hpp"
#include <string>

class StatusManager : public BaseManager {
public: // methods
    StatusManager(OrderManager& orders);

    void tick(const Symbol& symbol);

protected: // methods
    double getChange(const Symbol& symbol);
};

