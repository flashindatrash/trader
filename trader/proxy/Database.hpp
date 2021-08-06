#pragma once

#include "global.hpp"
#include "Proxy.hpp"

struct redisContext;
struct redisReply;

class Database : public Core::Proxy<Database>
{

public: // methods
    Database() {}
    virtual ~Database() override;

    void init();

    void set(const std::string& key, const char* value);
    void set(const std::string& key, int value);
    void set(const std::string& key, double value);
    void set(const std::string& key, bool value);

    std::vector<std::string> keys(const std::string& pattern);

    int incr(const std::string& key);

    void del(const std::string& key);

    const char* getAsString(const std::string& key);
    int getAsInt(const std::string& key);
    double getAsDouble(const std::string& key);
    bool getAsBool(const std::string& key);

    redisReply* cmd(const char *format, ...);

protected: // vars
    redisContext* _context = nullptr;
};

#define DB() Database::getInstance()
