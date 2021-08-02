#pragma once

#include "Proxy.hpp"
#include <unordered_map>
#include <string>

class BinancePrices : public Core::Proxy<BinancePrices>
{
public: // methods
    BinancePrices();

    void setPrice(const std::string& symbol, double price);
    double getPrice(const std::string& symbol) const;

protected: // methods
    bool init() override;

protected: // vars
    std::unordered_map<std::string, double> _prices;
};

#define SBinancePrices() BinancePrices::getInstance()
