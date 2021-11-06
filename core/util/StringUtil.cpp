#include "StringUtil.hpp"

std::string util::format(const char* fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    char buffer[512];
    size_t size = std::vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    if (size == 0)
        return "";

    return {buffer};
}

std::string util::lowercase(const char* chars) {
    std::string result(chars);
    for (char& ch : result)
        ch = tolower(ch);
    return result;
}

std::string util::uppercase(const char* chars) {
    std::string result(chars);
    for (char& ch : result)
        ch = toupper(ch);
    return result;
}