#pragma once

#include "exchanger/base/Identifier.hpp"
#include "exchanger/base/ExchangerTypes.hpp"

struct BinanceSymbolData;

class Asset : public std::string
{
public: // methods
    Asset();
    Asset(const std::string& asset);

    const double& getBalance() const;
};

class Symbol : public Identifier {
public: // static
    static Symbol* create();

public: // methods
    Symbol() = default;
    Symbol(const std::string& symbol);
    Symbol(Asset base, Asset second);

    void set(Asset base, Asset second);
    const Asset& baseAsset() const;
    const Asset& quoteAsset() const;

    const Price& getPrice() const;
    const Price getPrice(double quantity) const;

protected: // vars
    Asset _base;
    Asset _quote;
};
