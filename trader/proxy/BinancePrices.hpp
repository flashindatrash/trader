#pragma once

#include "Proxy.hpp"

class TradeSymbol;
class PriceSymbol;
struct BinancePriceStatisticsData;

class BinancePrices : public core::Proxy<BinancePrices>
{
public: // methods
    BinancePrices() = default;
    virtual ~BinancePrices() override;

    void init();

    // get 24hr statistics
    const BinancePriceStatisticsData& getStats(const TradeSymbol& symbol);

    // get price wrapper
    const PriceSymbol* getPrice(const TradeSymbol& symbol) const;
    PriceSymbol* getMutablePrice(const TradeSymbol& symbol) const;

protected: // vars
    std::unordered_map<std::string, PriceSymbol*> _symbols;
};

#define SPrices() BinancePrices::getInstance()
