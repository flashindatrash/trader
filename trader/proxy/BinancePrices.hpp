#pragma once

#include "Proxy.hpp"

class Symbol;
class PriceContainer;
struct BinancePriceStatisticsData;

class BinancePrices : public core::Proxy<BinancePrices>
{
public: // methods
    BinancePrices() = default;
    virtual ~BinancePrices() override;

    void init();

    // get 24hr statistics
    const BinancePriceStatisticsData& getStats(const Symbol& symbol);

    // get price wrapper
    const PriceContainer* getPrice(const Symbol& symbol) const;
    PriceContainer* getMutablePrice(const Symbol& symbol) const;

protected: // vars
    std::unordered_map<std::string, PriceContainer*> _symbols;
};

#define SPrices() BinancePrices::getInstance()
