#pragma once

#include <global.hpp>
#include "exchanger/wrapper/PriceWrapper.hpp"

struct BinanceSymbolData;

class Asset : public std::string
{
public: // methods
    Asset();
    Asset(const std::string& asset);

    const double& getBalance() const;
};

class Symbol : public std::string
{
public: // methods
    Symbol();
    Symbol(const std::string& symbol);
    Symbol(Asset base, Asset second);

    const Asset& baseAsset() const;
    const Asset& quoteAsset() const;

    const BinanceSymbolData& getInfo() const;

    const Price getPrice() const;
    const Price getPrice(double quantity) const;

protected: // vars
    Asset _base;
    Asset _quote;
};
