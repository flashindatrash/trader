#pragma once

#include <unordered_map>
#include "Types.hpp"
#include "Value.hpp"

namespace db {
class Object {
public: // static
    typedef std::unordered_map<Key, Value> Map;

public: // methods
    Object(Key key);
    Object() = default;

    void set(const Key& field, const Value& value);
    Value inc(const Key& field, const Value& value);
    Value get(const Key& field) const;

    size_t size() const;
    bool empty() const;

    bool remove();
    bool save();

public: // operators
    inline bool operator==(const Object& rhs) const { return _key == rhs._key; }
    inline bool operator!=(const Object& rhs) const { return !(*this == rhs); }
    inline operator const Value() const { return Value(_key); }

protected: // methods
    void load();

protected: // vars
    Key _key;
    Map _map;

    // have local changes
    bool _invalidated = false;
};
}
