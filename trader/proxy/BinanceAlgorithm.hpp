#pragma once

#include "Proxy.hpp"
#include "data/BinanceBalanceData.hpp"

class TradeSymbol;
class OrderManager;
class ProfitManager;
class TraderManager;
struct BinanceBookData;
struct BinanceKlineData;

class BinanceAlgorithm : public Core::Proxy<BinanceAlgorithm>
{
public: // methods
    BinanceAlgorithm() {}
    virtual ~BinanceAlgorithm() override;

    void init(const TradeSymbol& symbol);

protected: // methods
    void onBalanceChanged(const BinanceBalanceData &data);
    void onBookChanged(const BinanceBookData &data);
    void onKlineChanged(const BinanceKlineData &data);

protected: // vars
    OrderManager* _pool = nullptr;
    ProfitManager* _profit_manager = nullptr;
    TraderManager* _trader_manager = nullptr;

    std::vector<BinanceBalanceData> _balances;
};

#define SAlgorithm() BinanceAlgorithm::getInstance()
