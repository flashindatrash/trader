#include "Array.hpp"
#include "Database.hpp"

using namespace database;

Array::Array(const Key& key)
    : _key(key)
{
}

size_t Array::push(const Value& value) {
    size_t db_size = DB().rpush(_key, value);
    size_t local_size = size() + 1;
    if (db_size == local_size)
        internal_add(value);
    else load();
    return _values.size();
}

size_t Array::remove(const Value& value) {
    size_t db_removed = DB().lrem(_key, value);
    size_t local_removed = 0;
    for (Vector::iterator it = _values.begin(); it < _values.end(); ++it) {
        if (value.asString() == it->asString()) {
            ++local_removed;
            it = internal_remove(it);
        }
    }

    // remote size don't equal
    if (db_removed != local_removed)
        load();

    return db_removed;
}

bool Array::has(const Vector& vec, const Value& value) {
    for (const Value& it : vec) {
        if (it.asString() == value.asString())
            return true;
    }
    return false;
}

bool Array::has(const Value& value) const {
    return has(_values, value);
}

size_t Array::size() const {
    return _values.size();
}

void Array::internal_add(const Value& value) {
    _values.push_back(value);
    onAdded.emmit(value);
}

Array::Vector::iterator Array::internal_remove(Vector::iterator it) {
    onRemoved.emmit(*it);
    return _values.erase(it);
}

void Array::load() {
    if (_key.empty())
        return;

    const Vector upd = DB().lrange(_key);

    // remove old items
    for (Vector::iterator it = _values.begin(); it < _values.end(); ++it)
        if (not has(upd, *it))
            it = internal_remove(it);

    // add new items
    for (Vector::const_iterator it = upd.begin(); it < upd.end(); ++it)
        if (not has(*it))
            internal_add(*it);
}
