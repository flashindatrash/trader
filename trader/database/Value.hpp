#pragma once

#include <string>

namespace db {
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

public: // operators
    inline bool operator==(const Value& rhs) const { return asString() == rhs.asString(); }
    inline bool operator!=(const Value& rhs) const { return !(*this == rhs); }

protected: // vars
    std::string _value = "";
};
}
