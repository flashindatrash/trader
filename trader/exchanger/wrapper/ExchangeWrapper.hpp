#pragma once

#include "exchanger/base/Storage.hpp"
#include "exchanger/base/Symbol.hpp"

class ExchangeWrapper : public MapIdentifier<std::string> {
public: // static
    static ExchangeWrapper* create();

public: // methods
    void setAssets(const Asset& base, const Asset& quote);
    const Asset& baseAsset() const;
    const Asset& quoteAsset() const;

protected: // methods
    ExchangeWrapper() = default;

protected: // vars
    Asset _baseAsset;
    Asset _quoteAsset;
};

