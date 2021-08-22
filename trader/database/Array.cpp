#include "Array.hpp"
#include "Database.hpp"

using namespace db;

Array::Array(const Key& key)
    : ArrayAbstract(key)
{
    load();
}

Objects::Objects(const Key& key)
    : ArrayAbstract(key)
{
    load();
}

Value Array::interpolate(const Value& value) const {
    return value;
}

Object Objects::interpolate(const Value& value) const {
    return Object(value.asString());
}


size_t ArrayDatabase::rpush(const Key& key, const Value& value) {
    return DB().rpush(key, value);
}

std::vector<std::string> ArrayDatabase::lrange(const Key& key, int start, int stop) {
    return DB().lrange(key, start, stop);
}

size_t ArrayDatabase::lrem(const Key& key, const Value& value, int count) {
    return  DB().lrem(key, value, count);
}
