#pragma once

class BinanceSymbol;
class OrderPool;
class ProfitManager;
class TraderManager;
struct BinanceBalanceData;
struct BinanceBookData;

class Algorithm
{

public: // methods
    Algorithm(const BinanceSymbol& symbol);
    virtual ~Algorithm();

    void onBookChanged(const BinanceBookData &data);
    void onBalanceChanged(const BinanceBalanceData &data);

protected: // vars
    OrderPool* _pool = nullptr;
    ProfitManager* _profit_manager = nullptr;
    TraderManager* _trader_manager = nullptr;
};

