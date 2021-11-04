//
// Created by Вадим Проскурин on 05.11.2021.
//

#include "Events.hpp"
#include <cstdarg>

NS_USE

Events* Events::create(const db::Key& key) {
    auto* events = new Events(key);
    return events;
}

Events::Events(const db::Key& key)
    : db::Array(key)
{
}

bool Events::send(const char* fmt, ...) {
    va_list arg;
    va_start(arg, fmt);
    char buffer[256];
    size_t size = std::vsnprintf(buffer, sizeof(buffer), fmt, arg);
    va_end(arg);

    if (size == 0)
        return false;

    return push(buffer);
}