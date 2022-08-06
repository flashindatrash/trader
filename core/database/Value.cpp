#include "Value.hpp"

using namespace db;

Value Value::Empty;

Value::Value(const Value& value) {
    _type = value._type;
    _value = value._value;
}

Value::Value(const char* value)
    : Value(std::string(value))
{
}

Value::Value(const String value) {
    _type = TypeString;
    _value.str = value;
}

Value::Value(const Int value) {
    _type = TypeInt;
    _value.numeric = value;
    _value.str = std::to_string(value);
}

Value::Value(const Double value) {
    _type = TypeDouble;
    _value.numeric = value;
    _value.str = std::to_string(value);
}

Value::Value(const Bool value) {
    _type = TypeInt;
    _value.numeric = value ? 1 : 0;
    _value.str = std::to_string(asInt());
}

const char* Value::asCString() const {
    return _value.str.c_str();
}

Value::String Value::asString() const {
    return _value.str;
}

Value::Int Value::asInt() const {
    if (_type == TypeString)
        return atoi(asCString());
    return _value.numeric;
}

Value::Double Value::asDouble() const {
    if (_type == TypeString)
        return atof(asCString());
    return _value.numeric;
}

Value::Bool Value::asBool() const {
    return asInt() == 1;
}

size_t Value::size() const {
    return _value.str.size();
}

bool Value::operator==(const Value& rhs) const {
    return _type == rhs._type && asString() == rhs.asString();
}

Value& Value::operator+=(const Value& rhs) {
    _value.numeric += rhs._value.numeric;
    _value.str = std::to_string(_value.numeric);
    return *this;
}

Value Value::parse(const char* value, unsigned long len) {
    bool numeric = true;
    bool precision = false;
    for (unsigned long c = 0; c < len; ++c) {
         if (value[c]=='.') {
            if (!precision && c != 0 && c != len - 1) {
                precision = true;
            } else {
                numeric = false;
                break;
            }
         }
         else if (!isdigit(value[c])) {
              numeric = false;
              break;
         }
    }

    if (numeric) {
        if (precision)
            return Value(atof(value));
        else
            return Value(atoi(value));
    }

    return Value(value);
}
