#pragma once

#include <string>

typedef double Price;
typedef double Change;

namespace Changes {
    bool empty(Change change);
    bool equal(Change left, Change right);
}

class PriceRange {

public:  // methods
    PriceRange() = default;
    PriceRange(Change change);
    PriceRange(Price left, Price right);

    Change change() const;
    Change abs() const;

protected: // vars
    Change _change;
};

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
