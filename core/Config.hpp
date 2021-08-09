#pragma once

#include <unordered_map>
#include <string>

// todo move to config
#define BINANCE_TEST_MODE   false
#define BINANCE_RECV_WINDOW 3000

namespace core {

class Config {
protected: // static
    static const std::string sEmpty;

public: // methods
    Config() {}

    bool read(const char* path);

    const std::string& getAsString(const std::string& key) const;
    int getAsInt(const std::string& key) const;

protected: // vars
    std::unordered_map<std::string, std::string> _values;
};

}
