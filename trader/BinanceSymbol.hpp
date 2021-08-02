#pragma once

#include <string>

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
    BinanceSymbol(BinanceAsset base, BinanceAsset second);

    const BinanceAsset& baseAsset() const;
    const BinanceAsset& secondAsset() const;
    const double getPrice() const;

protected: // vars
    BinanceAsset _base;
    BinanceAsset _second;
};
