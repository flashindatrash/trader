#include "Config.hpp"
#include <fstream>

using namespace core;

const std::string Config::sEmpty = "";

bool Config::read(const char* path) {
    // std::ifstream is RAII, i.e. no need to call close
    std::ifstream cFile(path);
    if (not cFile.is_open())
        return false;

    std::string line;
    while(getline(cFile, line)){
        line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
        if(line[0] == '#' || line.empty())
            continue;
        auto delimiterPos = line.find("=");
        auto name = line.substr(0, delimiterPos);
        auto value = line.substr(delimiterPos + 1);
        _values[name] = value;
    }
    return true;
}

const std::string& Config::getAsString(const std::string& key) const {
    auto it = _values.find(key);
    if (it == _values.end())
        return sEmpty;

    return it->second;
}

int Config::getAsInt(const std::string& key) const {
    return atoi(getAsString(key).c_str());
}
