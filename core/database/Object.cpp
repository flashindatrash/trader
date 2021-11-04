#include "Object.hpp"

#include <utility>
#include "Database.hpp"

using namespace db;

Object::Object(Key  key)
    : _key(std::move(key))
{
    load();
}

void Object::set(const Key& field, const Value& value) {
    // todo: check if there were any changes
    _map[field] = value;
    _invalidated = true;
}

Value Object::inc(const Key& field, const Value& value) {
    // todo: check if there were any changes
    auto it = _map.find(field);
    if (it == _map.end()) {
        _map[field] = value;
        _invalidated = true;
        return value;
    }

    it->second += value;
    _invalidated = true;
    return it->second;
}

Value Object::get(const Key& field) const {
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

bool Object::remove() {
    if (DB().del(_key)) {
        _map.clear();
        return true;
    }
    return false;
}

bool Object::save() {
    if (!_invalidated)
        return true;

    if (not DB().hmset(_key, _map))
        return false;

    _invalidated = false;
    return true;
}

void Object::load() {
    if (_key.empty())
        return;

    // todo: overwrite only the fields you need
    _map = DB().hgetall(_key);
}
