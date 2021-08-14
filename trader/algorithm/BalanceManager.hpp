#pragma once

#include "exchanger/wrapper/PriceWrapper.hpp"
#include "exchanger/binance/response/BinanceBalanceData.hpp"

class Asset;
class Symbol;

class BalanceManager
{
public: // methods
    BalanceManager() = default;

    void add(const Asset& asset);
    void tick(const Symbol& symbol);

protected: // vars
    std::unordered_map<std::string, Price> _balances;
};

