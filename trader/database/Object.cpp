#include "Object.hpp"
#include "Database.hpp"

using namespace database;

Object::Object(const Key& key)
    : _key(key)
{
}

void Object::set(const Key& field, const Value& value) {
    _map[field] = value;
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

void Object::remove() {
    DB().del(_key);
}

bool Object::flush() {
    return DB().hmset(_key, _map);
}

void Object::load() {
    if (_key.empty())
        return;

    _map = DB().hgetall(_key);
}
