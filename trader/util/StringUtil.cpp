#include "StringUtil.hpp"

const std::string util::lowercase(const char* chars) {
    std::string result(chars);
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = tolower(result[i]);
    return result;
}

const std::string util::uppercase(const char* chars) {
    std::string result(chars);
    for (size_t i = 0; i < result.size(); ++i)
        result[i] = toupper(result[i]);
    return result;
}
