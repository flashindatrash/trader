#pragma once

#include "Identifier.hpp"
#include "OrderBase.hpp"

class Asset : public Identifier {
public: // static
    static const Asset Empty;
    static const Asset USDT;
    static const Asset BUSD;

public: // methods
    Asset() = default;
    Asset(const std::string& asset);

    const Quantity& balance() const;
    Quantity balance(const Asset& asset) const;
    Quantity convert(Quantity quantity, const Asset& asset = Asset::USDT) const;
    bool isUSD() const;

    const char* c_str() const;
    operator std::string() const;

    std::string operator+(const Asset& quote) const;
};
