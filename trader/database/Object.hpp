#pragma once

#include <unordered_map>
#include "Types.hpp"
#include "Value.hpp"

namespace database {
class Object {
public: // methods
    Object() = default;

    void set(const Key field, const Value value);
    Value get(const Key field) const;
    Value& get(const Key field);

    size_t size() const;
    bool empty() const;

protected: // vars
    std::unordered_map<Key, Value> _map;
};
}
