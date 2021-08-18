#pragma once

#include "time.h"
#include "strategy/Strategy.hpp"
#include "exchanger/wrapper/Symbol.hpp"

class OrderManager;
class StatusManager;
class ProfitManager;
class TraderManager;

class TraderAlgorithm : public Strategy {
public: // methods
    virtual bool init(const core::Config& config) override;

    void tick(time_t now);

protected: // methods
    TraderAlgorithm() = default;
    virtual ~TraderAlgorithm() override;

protected: // vars
    Symbol _symbol;

    OrderManager* _pool = nullptr;
    StatusManager* _status_manager = nullptr;
    ProfitManager* _profit_manager = nullptr;
    TraderManager* _trader_manager = nullptr;
};

#define SAlgorithm() TraderAlgorithm::getInstance()
