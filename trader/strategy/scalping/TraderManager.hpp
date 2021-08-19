#pragma once

#include "BaseManager.hpp"

class CandlestickWrapper;

namespace scalping {

class TraderManager : public BaseManager {
public: // methods
    TraderManager(OrderManager& orders);

    bool init(const Symbol& symbol) override;

protected: // methods
    // свеча закрылась
    void onCloseCandle(const CandlestickWrapper& wrapper);
};

}
