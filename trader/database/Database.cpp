#include "Database.hpp"
#include "Config.hpp"
#include "Logger.hpp"
#include "hiredis/hiredis.h"

using namespace db;

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
    return receive(result);
}

redisReply* Database::cmdArgv(int argc, const char **argv, const size_t *argvlen) {
    return receive(redisCommandArgv(_context, argc, argv, argvlen));
}

redisReply* Database::receive(void* reply) const {
    if (reply == nullptr) {
        Logger::info("redisReply is null");
        return nullptr;
    }

    redisReply* result = (redisReply*)reply;
    if (result->type == REDIS_REPLY_ERROR) {
        Logger::info("redisReply: %s", result->str);
        return nullptr;
    }

    return result;
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

size_t Database::rpush(const Key& key, const Value& value) {
    if (redisReply* result = cmd("RPUSH %s %s", key.c_str(), value.asCString())) {
        if (result->type == REDIS_REPLY_INTEGER)
            return result->integer;
    }
    return 0;
}

VectorValues Database::lrange(const Key& key, int start/* = 0*/, int stop/* = -1*/) {
    VectorValues vector;
    if (redisReply* result = cmd("LRANGE %s %d %d", key.c_str(), start, stop)) {
        if (result->type == REDIS_REPLY_ARRAY) {
            for (size_t i = 0; i < result->elements; ++i) {
                redisReply* item = result->element[i];
                vector.push_back(item->str);
            }
        }
    }
    return vector;
}

size_t Database::lrem(const Key& key, const Value& value, int count/* = 0*/) {
    if (redisReply* result = cmd("LREM %s %d %s", key.c_str(), count, value.asCString())) {
        if (result->type == REDIS_REPLY_INTEGER)
            return result->integer;
    }
    return 0;
}

bool Database::hmset(const Key& key, const Object::Map& map) {
    if (map.empty())
        return false;

    std::vector<const char *> argv;
    std::vector<size_t> argvlen;

    static char cmd[] = "HMSET";
    argv.push_back(cmd);
    argvlen.push_back(sizeof(cmd)-1);

    argv.push_back( key.c_str() );
    argvlen.push_back( key.size() );

    for (auto& it : map) {
        argv.push_back(it.first.c_str());
        argvlen.push_back(it.first.size());
        argv.push_back(it.second.asCString());
        argvlen.push_back(it.second.size());
    }

    if (redisReply* result = cmdArgv(argv.size(), &(argv[0]), &(argvlen[0]))) {
        if (result->type == REDIS_REPLY_STATUS)
            return true;
    }

    return false;
}

Object::Map Database::hgetall(const Key& key) {
    Object::Map map;
    if (redisReply* result = cmd("HGETALL %s", key.c_str())) {
        if (result->type == REDIS_REPLY_ARRAY) {
            Key field;
            for (size_t i = 0; i < result->elements; ++i) {
                redisReply* item = result->element[i];
                if (i % 2 == 0)
                    field = item->str;
                else
                    map.insert(std::make_pair(field, item->str));
            }
        }
    }
    return map;
}

int Database::incr(const Key& key) {
    if (redisReply* result = cmd("INCR %s", key.c_str())) {
        if (result->type == REDIS_REPLY_INTEGER)
            return result->integer;
    }
    return 0;
}

bool Database::del(const Key &key) {
    if (redisReply* result = cmd("DEL %s", key.c_str())) {
        if (result->type == REDIS_REPLY_INTEGER)
            return true;
    }
    return false;
}

std::vector<Key> Database::keys(const std::string& pattern) {
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
