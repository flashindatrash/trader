//
// Created by Вадим Проскурин on 01.08.2022.
//

#include "Decimal.hpp"
#include <cmath>
#include <cctype>

const size_t Decimal::default_size = 8;

Decimal::IntType Decimal::deserialize(const StrType& value, size_t size) {
    IntType result = 0;

    std::size_t len = value.find('.');
    if (len == std::string::npos)
        len = value.size();

    for (size_t i = 0, j = len + size; i < value.size() && j > 0; ++i) {
        const char& ch = value[i];

        if (not isdigit(ch))
            continue;

        IntType digit = ch - '0';
        result += digit * (IntType)std::pow(10, --j);
    }

    return result;
}

Decimal::StrType Decimal::serialize(const IntType& value, size_t size) {
    StrType result = std::to_string(value);

    size_t min = size + 1;
    if (result.size() < min)
        result.insert(0, min - result.size(), '0');

    result.insert(result.size() - size, ".");
    return result;
}

Decimal::Decimal(IntType value)
    : _value(value)
{
}

const char* Decimal::c_str() const {
    return ((const StrType&)*this).c_str();
}

Decimal::operator Decimal::FloatType() const {
    return std::atof(c_str());
}

Decimal::operator Decimal::IntType() const {
    return _value;
}

Decimal::operator const Decimal::StrType&() const {
    static StrType serialized;
    serialized = serialize(_value, default_size);
    return serialized;
}

Decimal Decimal::operator+(const Decimal &rhs) const {
    return (IntType)(*this) + (IntType)rhs;
}

Decimal Decimal::operator-(const Decimal &rhs) const {
    return (IntType)(*this) - (IntType)rhs;
}

Decimal Decimal::operator*(const Decimal &rhs) const {
    return (IntType)(*this) * (IntType)rhs;
}

Decimal Decimal::operator/(const Decimal &rhs) const {
    return (IntType)(*this) / (IntType)rhs;
}

Decimal& Decimal::operator+=(const Decimal &rhs) {
    _value += (IntType)rhs;
    return *this;
}

Decimal& Decimal::operator-=(const Decimal &rhs) {
    _value -= (IntType)rhs;
    return *this;
}

Decimal Decimal::operator*(double rhs) const {
    return (IntType)((double)_value * rhs);
}

Decimal Decimal::operator/(double rhs) const {
    return (IntType)((double)_value / rhs);
}

bool Decimal::operator ==(const Decimal& rhs) const {
    return (IntType)(*this) == (IntType)rhs;
}

bool Decimal::operator !=(const Decimal& rhs) const {
    return (IntType)(*this) != (IntType)rhs;
}

bool Decimal::operator <(const Decimal& rhs) const {
    return (IntType)(*this) < (IntType)rhs;
}

bool Decimal::operator >(const Decimal& rhs) const {
    return (IntType)(*this) > (IntType)rhs;
}

bool Decimal::operator ==(int rhs) const {
    return *this == Decimal(rhs);
}

bool Decimal::operator !=(int rhs) const {
    return *this != Decimal(rhs);
}

bool Decimal::operator <(int rhs) const {
    return *this < Decimal(rhs);
}

bool Decimal::operator >(int rhs) const {
    return *this > Decimal(rhs);
}

bool Decimal::operator <=(int rhs) const {
    return *this < Decimal(rhs);
}

bool Decimal::operator >=(int rhs) const {
    return *this > Decimal(rhs);
}