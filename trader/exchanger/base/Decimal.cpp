//
// Created by Вадим Проскурин on 01.08.2022.
//

#include "Decimal.hpp"
#include <cmath>
#include <cctype>
#include <unordered_map>

const size_t Decimal::default_size = 8;
const Decimal Decimal::Zero;

Decimal::IntType Decimal::deserialize(const StrType& value, size_t size) {
    IntType result = 0;
    if (value.empty())
        return result;

    std::size_t len = value.find('.');
    if (len == std::string::npos)
        len = value.size();

    size_t first = 0;
    bool negative = false;
    if (value.front() == '-') {
        ++first; --len;
        negative = true;
    }

    for (size_t i = first, j = len + size; i < value.size() && j > 0; ++i) {
        const char& ch = value[i];

        if (not isdigit(ch))
            continue;

        IntType digit = ch - '0';
        result += digit * (IntType)std::pow(10, --j);
    }

    if (negative)
        result *= -1LL;

    return result;
}

Decimal::StrType Decimal::serialize(const IntType& value, size_t size) {
    StrType result = std::to_string(value);

    size_t len = result.size();
    size_t first = 0;
    if (not result.empty() && result.front() == '-') {
        ++first; --len;
    }

    size_t min = size + 1;
    if (len < min)
        result.insert(first, min - len, '0');

    result.insert(result.size() - size, ".");
    return result;
}

Decimal::Decimal(IntType value)
    : _value(value)
{
}

Decimal::Decimal(FloatType value)
    : _value(std::llround(value * std::pow(10, default_size)))
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
    //static StrType t;
    //t = serialize(_value, default_size);
    //return t;
    static std::unordered_map<IntType, StrType> cache;

    auto it = cache.find(_value);
    if (it != cache.end())
        return it->second;

    StrType serialized = serialize(_value, default_size);
    cache.insert(std::make_pair(_value, serialized));
    return cache.at(_value);
}

Decimal Decimal::operator+(const Decimal &rhs) const {
    return (IntType)(*this) + (IntType)rhs;
}

Decimal Decimal::operator-(const Decimal &rhs) const {
    return (IntType)(*this) - (IntType)rhs;
}

Decimal Decimal::operator*(const Decimal &rhs) const {
    return *this * (FloatType)rhs;
}

Decimal Decimal::operator/(const Decimal &rhs) const {
    return *this / (FloatType)rhs;
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
    return (IntType)((FloatType)_value * rhs);
}

Decimal Decimal::operator/(double rhs) const {
    return (IntType)((FloatType)_value / rhs);
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

bool Decimal::operator <=(const Decimal& rhs) const {
    return (IntType)(*this) <= (IntType)rhs;
}

bool Decimal::operator >=(const Decimal& rhs) const {
    return (IntType)(*this) >= (IntType)rhs;
}
