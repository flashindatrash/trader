#pragma once

#include "Proxy.hpp"
#include "data/BinancePriceStatisticsData.hpp"

class TradeSymbol;
class PriceHistory;
struct BinanceBookData;

class BinancePrices : public core::Proxy<BinancePrices>, public core::Emitter<BinanceBookData>
{
public: // methods
    BinancePrices() {}
    virtual ~BinancePrices() override;

    void init();
    void connect(const TradeSymbol& symbol);

    BinancePriceStatisticsData getPriceStatistics(const TradeSymbol& symbol);

    // last average price
    void setPrice(const TradeSymbol& symbol, double price);
    double getPrice(const TradeSymbol& symbol) const;

    // price history
    const PriceHistory* getHistory(const TradeSymbol& symbol) const;

protected: // methods
    int handle(Json::Value& json);

protected: // vars
    std::unordered_map<std::string, double> _prices;
    std::unordered_map<std::string, PriceHistory*> _histories;
};

#define SPrices() BinancePrices::getInstance()
