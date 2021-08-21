#include "Database.hpp"

#include "Config.hpp"
#include "Logger.hpp"
#include "hiredis/hiredis.h"

using namespace database;

Database::~Database() {
    if (_context != nullptr) {
        redisFree(_context);
        _context = nullptr;
    }
}

bool Database::init(const core::Config& config) {
    _context = redisConnect(config.getAsString("REDIS_HOST").c_str(), config.getAsInt("REDIS_PORT"));
    if (_context == nullptr) {
        Logger::error("can't allocate redis context\n");
        return false;
    } else if (_context->err) {
        Logger::error(_context->errstr);
        return false;
    }
    return true;
}

redisReply* Database::cmd(const char* format, ...) {
    va_list args;
    va_start(args, format);
    void* result = redisvCommand(_context, format, args);
    va_end(args);

    if (result == nullptr) {
        Logger::info("result Database::cmd null");
        return nullptr;
    }

    return (redisReply*)result;
}

void Database::set(const Key& key, const Value& value) {
    cmd("SET %s %s", key.c_str(), value.asCString());
}

const Value Database::get(const std::string& key) {
    if (redisReply* result = cmd("GET %s", key.c_str())) {
        if (result->type == REDIS_REPLY_STRING)
            return result->str;
    }
    return Value::Empty;
}

int Database::rpush(const Key& key, const Value& value) {
    if (redisReply* result = cmd("RPUSH %s %s", key.c_str(), value.asCString())) {
        if (result->type == REDIS_REPLY_INTEGER)
            return result->integer;
    }
    return 0;
}

std::vector<Value> Database::lrange(const Key& key, int start/* = 0*/, int stop/* = -1*/) {
    std::vector<Value> arr;
    if (redisReply* result = cmd("LRANGE %s %d %d", key.c_str(), start, stop)) {
        if (result->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < result->elements; ++i) {
                redisReply* item = result->element[i];
                arr.push_back(item->str);
            }
        }
    }
    return arr;
}

const Object Database::hgetall(const Key& key) {
    Object obj;
    if (redisReply* result = cmd("HGETALL %s", key.c_str())) {
        if (result->type == REDIS_REPLY_ARRAY) {
            Key field;
            for (size_t i = 0; i < result->elements; ++i) {
                redisReply* item = result->element[i];
                if (i % 2 == 0)
                    field = item->str;
                else
                    obj.set(field, item->str);
            }
        }
    }
    return obj;
}

int Database::incr(const Key& key) {
    if (redisReply* result = cmd("INCR %s", key.c_str())) {
        if (result->type == REDIS_REPLY_INTEGER)
            return result->integer;
    }
    return 0;
}

void Database::del(const Key &key) {
    cmd("DEL %s", key.c_str());
}

std::vector<database::Key> Database::keys(const std::string& pattern) {
    std::vector<Key> keys;
    if (redisReply* result = cmd("KEYS %s", pattern.c_str())) {
        if (result->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < result->elements; ++i) {
                redisReply* item = result->element[i];
                keys.push_back(item->str);
            }
        }
    }
    return keys;
}
