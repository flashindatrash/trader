//
// Created by Вадим Проскурин on 01.08.2022.
//

#pragma once

#include <string>

class Decimal {
private:
    typedef int64_t IntType;
    typedef std::string StrType;
    typedef double FloatType;

    static const size_t default_size;

public:
    static IntType deserialize(const StrType& value, size_t size = default_size);
    static StrType serialize(const IntType& value, size_t size = default_size);

public:
    Decimal() = default;
    Decimal(IntType value);

    const char* c_str() const;

    operator const StrType&() const;
    operator IntType() const;
    operator FloatType() const;

    Decimal operator +(const Decimal& rhs) const;
    Decimal operator -(const Decimal& rhs) const;
    Decimal operator *(const Decimal& rhs) const;
    Decimal operator /(const Decimal& rhs) const;

    Decimal& operator +=(const Decimal& rhs);
    Decimal& operator -=(const Decimal& rhs);

    Decimal operator *(double rhs) const;
    Decimal operator /(double rhs) const;

    bool operator ==(const Decimal& rhs) const;
    bool operator !=(const Decimal& rhs) const;
    bool operator <(const Decimal& rhs) const;
    bool operator >(const Decimal& rhs) const;

    bool operator ==(int rhs) const;
    bool operator !=(int rhs) const;
    bool operator <(int rhs) const;
    bool operator >(int rhs) const;
    bool operator <=(int rhs) const;
    bool operator >=(int rhs) const;

private:
    IntType _value = 0;
};


