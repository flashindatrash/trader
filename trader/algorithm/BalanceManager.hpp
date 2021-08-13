#pragma once

#include "exchanger/binance/response/BinanceBalanceData.hpp"

class TradeSymbol;

class BalanceManager
{
public: // methods
    BalanceManager() = default;

    void add(const BinanceBalanceData& balance);
    bool check(const TradeSymbol& symbol);

protected: // vars
    std::vector<BinanceBalanceData> _balances;
};

