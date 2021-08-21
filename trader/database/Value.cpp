#include "Value.hpp"

using namespace database;

Value Value::Empty = "";

Value::Value(const char* value)
    : _value(value)
{
}

Value::Value(const std::string& value)
    : Value(value.c_str())
{
}

Value::Value(const int value)
    : Value(std::to_string(value))
{
}

Value::Value(const double value)
    : Value(std::to_string(value))
{
}

Value::Value(const bool value)
    : Value(value ? "1" : "0")
{
}

const char* Value::asCString() const {
    return _value;
}

std::string Value::asString() const {
    return std::string(_value);
}

int Value::asInt() const {
    return atoi(asCString());
}

double Value::asDouble() const {
    return atof(asCString());
}

long Value::asLong() const {
    return atol(asCString());
}

bool Value::asBool() const {
    return asInt() == 1;
}
