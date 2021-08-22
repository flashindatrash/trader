#pragma once

#include "ArrayAbstract.hpp"
#include "Object.hpp"

namespace db {
class Array : public ArrayAbstract<Value> {
public: // methods
    Array(const Key& key);
    Array() = default;

protected: /// methods
    virtual Value interpolate(const Value& value) const override;
};

class ArrayObject : public ArrayAbstract<Object> {
public: // methods
    ArrayObject(const Key& key);
    ArrayObject() = default;

protected: // methods
    virtual bool proceed_push(Object& value) const override;
    virtual bool proceed_erase(Object& value) const override;
    virtual Object interpolate(const Value& value) const override;
};
}

