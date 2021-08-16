#pragma once

#include "exchanger/base/Identifier.hpp"
#include "exchanger/base/Symbol.hpp"

enum OrderType : unsigned int;

struct SymbolData {
    Asset baseAsset;
    Asset quoteAsset;

public: // virtuals
    virtual bool hasOrderType(const OrderType& type) const { return false; };
};

class ExchangeWrapper : public Identifier {
public: // static
    static ExchangeWrapper* create();

public: // methods
    void set(SymbolData data);

    const Asset& baseAsset() const;
    const Asset& quoteAsset() const;

    bool hasOrderType(const OrderType& type) const;

protected: // methods
    ExchangeWrapper() = default;

protected: // vars
    SymbolData _data;
};

