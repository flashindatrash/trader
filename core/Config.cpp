#include "Config.hpp"
#include <fstream>
#include <algorithm>
#include <utility>

using namespace core;

bool Config::read(const char* path) {
    // save file name
    std::string p(path);
    size_t slash = p.rfind('/', p.length());
    size_t dot = p.rfind('.', p.length());
    if (slash != std::string::npos && dot != std::string::npos && dot > slash) {
        _name = p.substr(slash + 1, p.length() - dot);
    }

    if (_name.empty())
        return false;

    // std::ifstream is RAII, i.e. no need to call close
    std::ifstream cFile(path);
    if (not cFile.is_open())
        return false;

    std::string line;
    while(getline(cFile, line)){
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if(line[0] == '#' || line.empty())
            continue;
        auto delimiterPos = line.find('=');
        auto name = line.substr(0, delimiterPos);
        auto value = line.substr(delimiterPos + 1);
        set(name, value);
    }
    return true;
}

void Config::set(const Key& key, std::string value) {
    _values[key] = std::move(value);
}

bool Config::has(const Key& key) const {
    return _values.find(key) != _values.end();
}

const std::string& Config::asString(const Key& key) const {
    static const std::string& sEmpty = "";

    auto it = _values.find(key);
    if (it == _values.end())
        return sEmpty;

    return it->second;
}

int Config::asInt(const Key& key) const {
    return atoi(asString(key).c_str());
}

bool Config::asBool(const Key& key) const {
    return asInt(key) == 1;
}

double Config::asDouble(const Key& key) const {
    return atof(asString(key).c_str());
}

const std::string &Config::name() const {
    return _name;
}

