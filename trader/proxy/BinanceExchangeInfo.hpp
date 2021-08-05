#pragma once

#include "Proxy.hpp"
#include "data/BinanceSymbolData.hpp"

class BinanceExchangeInfo : public Core::Proxy<BinanceExchangeInfo>
{
public: // methods
    BinanceExchangeInfo() {}

    void init();

    const BinanceSymbolData& getSymbolInfo(const std::string& symbol) const;

protected: // vars
    std::unordered_map<std::string, BinanceSymbolData> _symbols;
};

#define SExchangeInfo() BinanceExchangeInfo::getInstance()
