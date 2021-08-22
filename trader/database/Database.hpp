#pragma once

#include "Proxy.hpp"
#include "Types.hpp"
#include "Value.hpp"
#include "Array.hpp"
#include "Object.hpp"

namespace core {
    class Config;
}

struct redisContext;
struct redisReply;

namespace db {
class Database : public core::Proxy<Database> {

public: // methods
    Database() = default;
    virtual ~Database() override;

    bool init(const core::Config& config);

    void set(const Key& key, const Value& value);
    const Value get(const Key& key);

    size_t rpush(const Key& key, const Value& value);
    std::vector<std::string> lrange(const Key& key, int start = 0, int stop = -1);
    size_t lrem(const Key& key, const Value& value, int count = 0);

    bool hmset(const Key& key, Object::Map& map);
    Object::Map hgetall(const Key& key);

    int incr(const Key& key);
    bool del(const Key& key);

    std::vector<Key> keys(const std::string& pattern);

protected: // methods
    redisReply* cmd(const char *format, ...);
    redisReply* cmdArgv(int argc, const char **argv, const size_t *argvlen);

    redisReply* receive(void *reply) const;

protected: // vars
    redisContext* _context = nullptr;
};
}

#define DB() db::Database::getInstance()
