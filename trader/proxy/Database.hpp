#pragma once

#include "global.hpp"
#include "Proxy.hpp"

namespace core {
    class Config;
}

struct redisContext;
struct redisReply;

class Database : public core::Proxy<Database>
{

public: // methods
    Database() {}
    virtual ~Database() override;

    void init(const core::Config& config);

    void set(const std::string& key, const char* value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, long value);
    void set(const std::string& key, bool value);

    std::vector<std::string> keys(const std::string& pattern);

    int incr(const std::string& key);

    void del(const std::string& key);

    const char* getAsString(const std::string& key);
    int getAsInt(const std::string& key);
    double getAsDouble(const std::string& key);
    double getAsLong(const std::string& key);
    bool getAsBool(const std::string& key);

    redisReply* cmd(const char *format, ...);

protected: // vars
    redisContext* _context = nullptr;
};

#define DB() Database::getInstance()
