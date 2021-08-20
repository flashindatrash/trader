#pragma once

#include <unordered_map>
#include <string>

namespace core {

class Config {
    typedef std::string Key;

public: // methods
    Config() = default;

    bool read(const char* path);

    bool has(const Key& key) const;
    const std::string& getAsString(const Key& key) const;
    int getAsInt(const Key& key) const;
    bool getAsBool(const Key& key) const;

protected: // vars
    std::unordered_map<std::string, std::string> _values;
};

}
