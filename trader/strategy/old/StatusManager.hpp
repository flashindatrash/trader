#pragma once

#include "BaseManager.hpp"
#include <string>

namespace scalping {

class StatusManager : public BaseManager {
public: // methods
    StatusManager(OrderManager& orders);

    void tick(const Symbol& symbol);

protected: // methods
    double getChange(const Symbol& symbol);
};

}
