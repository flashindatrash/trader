#pragma once

#include "Symbol.hpp"

class SymbolInfo {
public: // static
    static SymbolInfo* create();

public: // methods
    void setAssets(const Asset& base, const Asset& quote);
    const Asset& baseAsset() const;
    const Asset& quoteAsset() const;

protected: // methods
    SymbolInfo() = default;

protected: // vars
    Asset _baseAsset;
    Asset _quoteAsset;
};

