#pragma once

#include "BaseManager.hpp"
#include <string>

class StatusManager : public BaseManager {
public: // methods
    StatusManager(OrderManager& orders);

    void tick(const Symbol& symbol);

protected: // methods
    std::string getTimline(double current);
    double getChange();
    int getZerosAfterDot(double num);

};

