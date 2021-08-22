#include "Array.hpp"

using namespace db;

Array::Array(const Key& key)
    : ArrayAbstract(key)
{
    load();
}

Value Array::interpolate(const Value& value) const {
    return value;
}

ArrayObject::ArrayObject(const Key& key)
    : ArrayAbstract(key)
{
    load();
}

bool ArrayObject::proceed_push(Object& value) const {
    return value.save();
}

bool ArrayObject::proceed_erase(Object& value) const {
    return value.remove();
}

Object ArrayObject::interpolate(const Value& value) const {
    return Object(value.asString());
}
