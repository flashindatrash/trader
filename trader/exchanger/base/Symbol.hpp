#pragma once

#include "Identifier.hpp"
#include "BaseTypes.hpp"
#include "Asset.hpp"

class Symbol : public Identifier {
public: // static
    static Symbol* create();

public: // methods
    Symbol() = default;
    Symbol(const std::string& symbol);
    Symbol(const Asset& base, const Asset& second);

    void set(const Asset& base, const Asset& second);
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
