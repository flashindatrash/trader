#pragma once

#include <global.hpp>
#include "Proxy.hpp"

class BinancePrices : public Core::Proxy<BinancePrices>
{
public: // methods
    BinancePrices() {}

    void init();

    void setPrice(const std::string& symbol, double price);
    double getPrice(const std::string& symbol) const;

protected: // vars
    std::unordered_map<std::string, double> _prices;
};

#define SBinancePrices() BinancePrices::getInstance()
