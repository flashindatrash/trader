#pragma once

#include "exchanger/binance/response/BinanceBalanceData.hpp"

class Symbol;

class BalanceManager
{
public: // methods
    BalanceManager() = default;

    void add(const BinanceBalanceData& balance);
    void print(const Symbol& symbol);

protected: // vars
    std::vector<BinanceBalanceData> _balances;
};

