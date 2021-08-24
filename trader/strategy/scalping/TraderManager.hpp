#pragma once

#include "BaseManager.hpp"

namespace scalping {

class TraderManager : public BaseManager {
public: // methods
    TraderManager(OrderManager& orders);

    bool init(const Symbol& symbol) override;
    void tick(const Symbol& symbol) override;
};

}
