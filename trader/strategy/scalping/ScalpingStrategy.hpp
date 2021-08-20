#pragma once

#include "time.h"
#include "strategy/Strategy.hpp"
#include "exchanger/base/Symbol.hpp"

namespace scalping {

class OrderManager;
class StatusManager;
class ProfitManager;
class TraderManager;

class ScalpingStrategy : public Strategy {
public: // methods
    ScalpingStrategy() = default;

    virtual bool init(const core::Config& config) override;
    virtual bool isRunning() const override;

    void tick(time_t now);

protected: // methods
    virtual ~ScalpingStrategy() override;

protected: // vars
    Symbol _symbol;

    OrderManager* _pool = nullptr;
    StatusManager* _status_manager = nullptr;
    ProfitManager* _profit_manager = nullptr;
    TraderManager* _trader_manager = nullptr;
};

}
