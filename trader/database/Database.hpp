#pragma once

#include <string>
#include "Proxy.hpp"
#include "Types.hpp"
#include "Value.hpp"
#include "Object.hpp"

namespace core {
    class Config;
}

struct redisContext;
struct redisReply;

namespace database {
class Database : public core::Proxy<Database> {

public: // methods
    Database() = default;
    virtual ~Database() override;

    bool init(const core::Config& config);

    void set(const Key& key, const Value& value);
    const Value get(const Key& key);

    const Object hgetall(const Key& key);

    void del(const Key& key);
    int incr(const Key& key);

    std::vector<Key> keys(const std::string& pattern);

protected: // methods
    redisReply* cmd(const char *format, ...);

protected: // vars
    redisContext* _context = nullptr;
};
}

#define DB() database::Database::getInstance()
