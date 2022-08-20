#pragma once

#include "Identifier.hpp"
#include "OrderBase.hpp"

class Asset : public Identifier {
public: // static
    static const Asset Empty;
    static const Asset USDT;
    static const Asset BUSD;
    static const Asset LD;

public: // methods
    Asset() = default;
    Asset(const std::string& asset);

    // balance
    const Quantity& balance() const;

    // balance in asset
    Quantity balance(const Asset& asset) const;

    // price in asset
    Quantity convert(Quantity quantity, const Asset& asset = Asset::USDT) const;

    bool isUSD() const;

    Asset origin() const;
    Asset ld() const;

    const char* c_str() const;
    operator std::string() const;

    std::string operator+(const Asset& quote) const;
};
