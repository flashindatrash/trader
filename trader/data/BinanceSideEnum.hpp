#pragma once

#include <global.hpp>

struct BinanceSideEnum
{
public: // static
    enum Side {
        Invalid,
        Sell,
        Buy
    };

public: // methods
    BinanceSideEnum() {};
    BinanceSideEnum(Side value);
    BinanceSideEnum(double change);
    BinanceSideEnum(const std::string& value);

    BinanceSideEnum reverse() const;

    const char* c_str() const;

public: // operators
    bool operator==(const BinanceSideEnum other) const {
        return other._value == _value;
    };

    bool operator!=(const BinanceSideEnum other) const {
        return other._value != _value;
    };

    bool operator==(BinanceSideEnum::Side other) const {
        return other == _value;
    };

protected: // vars
    Side _value = Invalid;
};

