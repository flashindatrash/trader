#pragma once

#include <global.hpp>

struct BinanceSymbolData;

class TradeAsset : public std::string
{
public: // methods
    TradeAsset();
    TradeAsset(const std::string& asset);

    const double getBalance() const;
};

class TradeSymbol : public std::string
{
public: // methods
    TradeSymbol();
    TradeSymbol(const std::string& symbol);
    TradeSymbol(TradeAsset base, TradeAsset second);

    const TradeAsset& baseAsset() const;
    const TradeAsset& quoteAsset() const;

    const BinanceSymbolData& getInfo() const;

    const double getPrice() const;
    const double getPrice(double quantity) const;

protected: // vars
    TradeAsset _base;
    TradeAsset _quote;
};
