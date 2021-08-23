#pragma once

#include <string>

namespace db {
class Value {
    typedef std::string String;
    typedef int Int;
    typedef double Double;
    typedef bool Bool;

    enum Type {
        TypeString,
        TypeInt,
        TypeDouble
    };

public: // static
    static Value Empty;
    static Value parse(const char* value, unsigned long len);

public: // methods
    Value() = default;
    Value(const Value& value);
    Value(const char* value);
    Value(const String value);
    Value(const Int value);
    Value(const Double value);
    Value(const Bool value);

    const char*     asCString() const;
    String          asString() const;
    Int             asInt() const;
    Double          asDouble() const;
    Bool            asBool() const;

    size_t size() const;

public: // operators
    bool operator==(const Value& rhs) const;
    inline bool operator!=(const Value& rhs) const { return !(*this == rhs); }

protected: // vars
    Type _type = TypeString;
    struct V {
        String str = "";
        Double numeric = 0.0;
    } _value;
};
}
