#pragma once

#include <vector>
#include "Types.hpp"
#include "Value.hpp"
#include "Signal.hpp"

namespace database {
class Array {
public: // static
    typedef std::vector<Value> Vector;
    typedef Signal<Value> SignalValue;

public: // methods
    Array(const Key& key);
    Array() = default;

    size_t push(const Value& value);
    size_t remove(const Value& value);

    static bool has(const Vector& vec, const Value& value);
    bool has(const Value& value) const;

    size_t size() const;

public: // signals
    SignalValue onAdded;
    SignalValue onRemoved;

protected: // methods
    virtual void load();

private: // methods
    void internal_add(const Value& value);
    Vector::iterator internal_remove(Vector::iterator it);

private: // vars
    Key _key;
    Vector _values;
};
}
