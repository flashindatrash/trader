#pragma once

#include "time.h"
#include "strategy/Strategy.hpp"
#include "exchanger/wrapper/Symbol.hpp"

namespace longshort {

class OrderManager;
class StatusManager;
class ProfitManager;
class TraderManager;

class LongShortStrategy : public Strategy {
public: // methods
    LongShortStrategy() = default;
    virtual bool init(const core::Config& config) override;

    void tick(time_t now);

protected: // methods
    virtual ~LongShortStrategy() override;

protected: // vars
    Symbol _symbol;

    OrderManager* _pool = nullptr;
    StatusManager* _status_manager = nullptr;
    ProfitManager* _profit_manager = nullptr;
    TraderManager* _trader_manager = nullptr;
};

}
