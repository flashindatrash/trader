#pragma once

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
    [[nodiscard]] const Asset& baseAsset() const;
    [[nodiscard]] const Asset& quoteAsset() const;

    [[nodiscard]] const Price& price(const OrderSide& side) const;
    [[nodiscard]] Quantity balance(const Asset& asset = Asset::USDT) const;

    [[nodiscard]] bool exists() const;

    [[nodiscard]] const char* c_str() const;

    operator std::string() const;

protected: // vars
    Asset _base;
    Asset _quote;
};
