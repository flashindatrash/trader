#pragma once

#include "exchanger/base/Identifier.hpp"
#include "exchanger/base/ExchangerTypes.hpp"

struct BinanceSymbolData;

class Asset : public Identifier {
public: // methods
    Asset() = default;
    Asset(const std::string& asset);

    const double& getBalance() const;

    const char* c_str() const;
    operator std::string() const;
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

    const char* c_str() const;
    operator std::string() const;

protected: // vars
    Asset _base;
    Asset _quote;
};
