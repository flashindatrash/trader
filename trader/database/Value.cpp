#include "Value.hpp"

using namespace db;

Value Value::Empty;

Value::Value(const std::string value)
    : _value(value)
{
}

Value::Value(const Value& value)
    : Value(value.asString())
{
}

Value::Value(const char* value)
    : Value(std::string(value))
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
    return _value.c_str();
}

std::string Value::asString() const {
    return _value;
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

size_t Value::size() const {
    return _value.size();
}
