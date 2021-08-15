#pragma once

#include "Proxy.hpp"

namespace core {
    class Config;
}

class Asset;
class Symbol;
class OrderManager;
class StatusManager;
class ProfitManager;
class TraderManager;

class BinanceAlgorithm : public core::Proxy<BinanceAlgorithm>
{
public: // methods
    BinanceAlgorithm() = default;
    virtual ~BinanceAlgorithm() override;

    void init(const core::Config& config, const Symbol& symbol);
    void tick(time_t now);

protected: // methods
    void onBalanceChanged(const Asset& asset);

protected: // vars
    const Symbol* _symbol;

    OrderManager* _pool = nullptr;
    StatusManager* _status_manager = nullptr;
    ProfitManager* _profit_manager = nullptr;
    TraderManager* _trader_manager = nullptr;
};

#define SAlgorithm() BinanceAlgorithm::getInstance()
