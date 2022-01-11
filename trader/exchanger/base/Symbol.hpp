#pragma once

#include "Identifier.hpp"
#include "Asset.hpp"
#include "core/Signal.hpp"

class PriceWrapper;

class Symbol : public Identifier {
public: // static
    static Signal<Symbol> onAdded;
    static Symbol* create();

public: // methods
    Symbol() = default;
    Symbol(const std::string& symbol);
    Symbol(const Asset& base, const Asset& second);

    void set(const Asset& base, const Asset& second);
    const Asset& baseAsset() const;
    const Asset& quoteAsset() const;

    const Price& price(const OrderSide& side) const;
    Quantity balance(const Asset& asset = Asset::USDT) const;

    const char* c_str() const;
    operator std::string() const;

protected: // vars
    Asset _base;
    Asset _quote;
};
