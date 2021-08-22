#include "ArrayAbstract.hpp"
#include "Database.hpp"

using namespace db;

size_t ArrayHelper::rpush(const Key& key, const Value& value) {
    return DB().rpush(key, value);
}

VectorValues ArrayHelper::lrange(const Key& key, int start, int stop) {
    return DB().lrange(key, start, stop);
}

size_t ArrayHelper::lrem(const Key& key, const Value& value, int count) {
    return  DB().lrem(key, value, count);
}
