#pragma once

#include <unordered_map>
#include <string>

namespace core {

class Config {
public: // methods
    Config() = default;

    bool read(const char* path);

    bool has(const std::string& key) const;
    const std::string& getAsString(const std::string& key) const;
    int getAsInt(const std::string& key) const;

protected: // vars
    std::unordered_map<std::string, std::string> _values;
};

}
