#pragma once

#include <unordered_map>
#include <string>

namespace core {

class Config {
    typedef std::string Key;

public: // methods
    Config() = default;

    bool read(const char* path);

    void set(const Key& key, std::string value);
    bool has(const Key& key) const;

    const std::string&  asString(const Key& key) const;
    int                 asInt(const Key& key) const;
    bool                asBool(const Key& key) const;
    double              asDouble(const Key& key) const;

protected: // vars
    std::unordered_map<std::string, std::string> _values;
};

}
