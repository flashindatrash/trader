#pragma once

#include "Proxy.hpp"

class TradeSymbol;
class OrderManager;
class BalanceManager;
class ProfitManager;
class TraderManager;
struct BinanceBalanceData;
struct BinanceKlineData;

class BinanceAlgorithm : public core::Proxy<BinanceAlgorithm>
{
public: // methods
    BinanceAlgorithm() {}
    virtual ~BinanceAlgorithm() override;

    void init(const TradeSymbol& symbol);
    void tick(const TradeSymbol& symbol);

protected: // methods
    void onBalanceChanged(const BinanceBalanceData &data);

protected: // vars
    OrderManager* _pool = nullptr;
    BalanceManager* _balance_manager = nullptr;
    ProfitManager* _profit_manager = nullptr;
    TraderManager* _trader_manager = nullptr;
};

#define SAlgorithm() BinanceAlgorithm::getInstance()
