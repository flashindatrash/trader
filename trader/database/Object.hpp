#pragma once

#include <unordered_map>
#include "Types.hpp"
#include "Value.hpp"

namespace database {
class Object {
public: // static
    typedef std::unordered_map<Key, Value> Map;

public: // methods
    Object(const Key& key);
    Object() = default;

    void set(const Key& field, const Value& value);
    Value get(const Key& field) const;

    size_t size() const;
    bool empty() const;

    bool remove();
    bool flush();

protected: // methods
    void load();

private: // vars
    Key _key;
    Map _map;
};
}
