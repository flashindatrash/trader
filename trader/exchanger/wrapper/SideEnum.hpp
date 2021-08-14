#pragma once

#include <global.hpp>

struct SideEnum
{
public: // static
    enum Side {
        Invalid,
        Sell,
        Buy
    };

public: // methods
    SideEnum() = default;
    SideEnum(Side value);
    SideEnum(double change);
    SideEnum(const std::string& value);

    SideEnum reverse() const;

    const char* c_str() const;

public: // operators
    bool operator==(const SideEnum other) const {
        return other._value == _value;
    };

    bool operator!=(const SideEnum other) const {
        return other._value != _value;
    };

    bool operator==(SideEnum::Side other) const {
        return other == _value;
    };

protected: // vars
    Side _value = Invalid;
};

