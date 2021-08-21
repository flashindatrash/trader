#pragma once

#include <string>

namespace database {
class Value {
public: // static
    static Value Empty;

public: // methods
    Value() = default;
    Value(const Value& value);
    Value(const char* value);
    Value(const std::string value);
    Value(const int value);
    Value(const double value);
    Value(const bool value);

    const char*     asCString() const;
    std::string     asString() const;
    int             asInt() const;
    double          asDouble() const;
    long            asLong() const;
    bool            asBool() const;

    size_t size() const;

protected: // vars
    std::string _value = "";
};
}
