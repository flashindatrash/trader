//
// Created by Вадим Проскурин on 30.08.2021.
//

#include "Logger.hpp"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <csignal>
#include <cstdarg>
#include <utility>
#include "Time.hpp"

time_t Logger::sTime = 0;
bool Logger::sStatus = false;

void Logger::title(const char* fmt, ...) {
    if (getenv("QT_TERMINAL") != nullptr)
        return;
    va_list arg;

    va_start(arg, fmt);

    std::cout << ESCSTART;
    vfprintf(stdout, fmt, arg);
    std::cout << ESCEND;
    fflush(stdout);

    va_end (arg);
}

void Logger::info(const char* fmt, ...) {
    if (sStatus) {
        erase();
        sStatus = false;
    }

    va_list arg;
    va_start(arg, fmt);
    vfprintf(stdout, format(fmt), arg);
    fflush(stdout);
    va_end (arg);
}

void Logger::status(const char* fmt, ...) {
    if (getenv("QT_TERMINAL") != nullptr)
        return;

    if (sStatus)
        erase();
    sStatus = true;

    va_list arg;
    va_start(arg, fmt);

    static char new_fmt[1024];
    sprintf(new_fmt, "%s\n", fmt);
    vfprintf(stdout, new_fmt, arg);
    fflush(stdout);
    va_end (arg);
}

void Logger::erase() {
    if (getenv("QT_TERMINAL") != nullptr)
        return;

    std::cout << CURSOR_START << CURSOR_UP << ERASE_LINE;
}

void Logger::error(const char* msg) {
    std::raise(SIGSEGV);
}

void Logger::setTime(time_t time) {
    sTime = time;
}

const char* Logger::format(const char* fmt) {
    static char new_fmt[1024];
    time_t t = sTime > 0 ? sTime : Time().sec();
    struct tm* now = localtime(&t);
    sprintf(new_fmt, "[%04d/%02d/%02d %02d:%02d:%02d] T: %s\n", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, fmt);
    return new_fmt;
}