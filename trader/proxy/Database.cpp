#include "Database.hpp"

#include "Config.hpp"
#include "Logger.hpp"
#include "hiredis/hiredis.h"

Database::~Database() {
    if (_context != nullptr) {
        redisFree(_context);
        _context = nullptr;
    }
}

void Database::init() {
    /* Create Redis context and establish connection */
    _context = redisConnect(REDIS_HOST, REDIS_PORT);
    if (_context == nullptr) {
        logic_error("can't allocate redis context\n");
    } else if (_context->err) {
        logic_error(_context->errstr);
    }
}

redisReply* Database::cmd(const char* format, ...) {
    va_list args;
    va_start(args, format);
    void* result = redisvCommand(_context, format, args);
    va_end(args);

    if (result == nullptr) {
        trace("result Database::cmd null\n");
        return nullptr;
    }

    return (redisReply*)result;
}

void Database::set(const std::string& key, const char* value) {
    cmd("SET %s %s", key.c_str(), value);
}

void Database::set(const std::string& key, int value) {
    cmd("SET %s %d", key.c_str(), value);
}

void Database::set(const std::string& key, double value) {
    cmd("SET %s %f", key.c_str(), value);
}

void Database::set(const std::string& key, long value) {
    cmd("SET %s %s", key.c_str(), std::to_string(value).c_str());
}

void Database::set(const std::string& key, bool value) {
    set(key, value ? 1 : 0);
}

int Database::incr(const std::string& key) {
    if (redisReply* result = cmd("INCR %s", key.c_str())) {
        if (result->type == REDIS_REPLY_INTEGER)
            return result->integer;
    }
    return 0;
}

void Database::del(const std::string &key) {
    cmd("DEL %s", key.c_str());
}

std::vector<std::string> Database::keys(const std::string& pattern) {
    std::vector<std::string> keys;
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

const char* Database::getAsString(const std::string& key) {
    if (redisReply* result = cmd("GET %s", key.c_str())) {
        if (result->type == REDIS_REPLY_STRING)
            return result->str;
    }
    return "";
}

int Database::getAsInt(const std::string& key) {
    return atoi(getAsString(key));
}

double Database::getAsDouble(const std::string& key) {
    return atof(getAsString(key));
}

double Database::getAsLong(const std::string& key) {
    return atol(getAsString(key));
}

bool Database::getAsBool(const std::string& key) {
    return getAsInt(key) == 1;
}
