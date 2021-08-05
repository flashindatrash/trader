#pragma once

#include <global.hpp>

struct BinanceSymbolData;

class BinanceAsset : public std::string
{
public: // methods
    BinanceAsset();
    BinanceAsset(const std::string& asset);

    const double getBalance() const;
};

class BinanceSymbol : public std::string
{
public: // methods
    BinanceSymbol();
    BinanceSymbol(const std::string& symbol);
    BinanceSymbol(BinanceAsset base, BinanceAsset second);

    const BinanceAsset& baseAsset() const;
    const BinanceAsset& quoteAsset() const;

    const BinanceSymbolData& getInfo() const;

    const std::string toLowerCase() const;

    const double getPrice() const;
    const double getPrice(double quantity) const;

protected: // vars
    BinanceAsset _base;
    BinanceAsset _quote;
};
