#include "Object.hpp"

using namespace database;

void Object::set(const Key field, const Value value) {
    _map.insert(std::make_pair(field, value));
}

Value Object::get(const Key field) const {
    auto it = _map.find(field);
    if (it == _map.end())
        return Value::Empty;
    return it->second;
}

Value& Object::get(const Key field) {
    auto it = _map.find(field);
    if (it == _map.end())
        return Value::Empty;
    return it->second;
}

size_t Object::size() const {
    return _map.size();
}

bool Object::empty() const {
    return _map.empty();
}
