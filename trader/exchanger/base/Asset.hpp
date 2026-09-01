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

    // balance
    [[nodiscard]] const Quantity& balance() const;

    // balance in asset
    [[nodiscard]] Quantity balance(const Asset& asset) const;

    // price in asset
    [[nodiscard]] Quantity convert(Quantity quantity, const Asset& asset = Asset::USDT) const;

    [[nodiscard]] bool isUSD() const;

    [[nodiscard]] const char* c_str() const;

    operator std::string() const;

    std::string operator+(const Asset& quote) const;
};
