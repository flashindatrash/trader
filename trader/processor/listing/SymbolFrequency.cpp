//
// Created by Вадим Проскурин on 09.04.2022.
//

#include "SymbolFrequency.hpp"
#include "core/Time.hpp"

bool SymbolFrequency::request() {
    static time_t last = 0;

    time_t ms = Time().ms();
    time_t passed = ms - last;

    time_t sec = Time().sec();
    struct tm* gm = gmtime(&sec);
    char now[sizeof("hh:mm:ss")];
    strftime(now, sizeof(now), "%H:%M:%S", gm);

    time_t interval = Timer::sMinute;
    if (between(now, "11:59:59", "12:00:05"))
        interval = Timer::sSecond;

    if (passed < interval)
        return false;

    last = ms;
    return true;
}

bool SymbolFrequency::between(const std::string& now, const std::string& begin, const std::string& end) {
    return (now >= begin) && (now <= end);
}