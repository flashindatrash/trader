#pragma once

#include "Identifier.hpp"
#include "OrderBase.hpp"

class Asset : public Identifier {
public: // static
    static const Asset Empty;
    static const Asset USDT;

public: // methods
    Asset() = default;
    Asset(const std::string& asset);

    const Quantity& balance() const;
    Quantity balance(const Asset& asset) const;

    const char* c_str() const;
    operator std::string() const;

    const std::string operator+(const Asset& quote) const;
};
