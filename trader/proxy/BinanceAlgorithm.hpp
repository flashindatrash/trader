#pragma once

#include "Proxy.hpp"

class TradeSymbol;
class OrderManager;
class BalanceManager;
class ProfitManager;
class TraderManager;
struct BinanceBalanceData;

class BinanceAlgorithm : public core::Proxy<BinanceAlgorithm>
{
public: // methods
    BinanceAlgorithm() = default;
    virtual ~BinanceAlgorithm() override;

    void init(const TradeSymbol& symbol);
    void tick(time_t now);

protected: // methods
    void onBalanceChanged(const BinanceBalanceData &data);

protected: // vars
    const TradeSymbol* _symbol;

    OrderManager* _pool = nullptr;
    BalanceManager* _balance_manager = nullptr;
    ProfitManager* _profit_manager = nullptr;
    TraderManager* _trader_manager = nullptr;
};

#define SAlgorithm() BinanceAlgorithm::getInstance()
