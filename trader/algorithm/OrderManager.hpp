#pragma once

#include "data/BinanceOrderData.hpp"

class TradeSymbol;
class TradeAsset;

class OrderManager
{
protected: // static
    static const std::string key(const BinanceOrderData& transaction);

public: // methods
    OrderManager(const TradeSymbol& symbol);

    bool create(const TradeSymbol& symbol, const std::string& side, double quantity, const BinanceOrderData* transaction);

    const std::vector<BinanceOrderData>& getOrders() const;
    const std::vector<BinanceOrderData>& getTransactions() const;

    time_t getLastTime() const;

protected: // methods
    // добавить статистику профита
    void addProfitStats(double profit, const TradeAsset& asset);
    void updateLastTime(const TradeSymbol& symbol);
    void open(const BinanceOrderData& transaction);
    void close(const std::string& transaction_id);

protected: // vars
    std::vector<BinanceOrderData> _orders;
    std::vector<BinanceOrderData> _positions;
    time_t _last_time = 0;
};

