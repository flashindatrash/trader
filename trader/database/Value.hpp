#pragma once

#include <string>

namespace database {
class Value {
public: // static
    static Value Empty;

public: // methods
    Value(const char* value);
    Value(const std::string& value);
    Value(const int value);
    Value(const double value);
    Value(const bool value);

    const char* asCString() const;
    std::string asString() const;
    int         asInt() const;
    double      asDouble() const;
    long        asLong() const;
    bool        asBool() const;

protected: // methods
    Value() = default;

protected: // vars
    const char* _value;
};
}
