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

void Logger::title(const std::string& value) {
    if (getenv("QT_TERMINAL") != nullptr)
        return;

    std::cout << ESCSTART << value << ESCEND;
}

void Logger::info(const std::string& value) {
    if (sStatus) {
        erase();
        sStatus = false;
    }

    std::cout << formatTime() << ": " << value << std::endl;
}

void Logger::status(const std::string& value) {
    if (getenv("QT_TERMINAL") != nullptr)
        return;

    if (sStatus)
        erase();
    sStatus = true;

    std::cout << value << std::endl;
}

void Logger::erase() {
    if (getenv("QT_TERMINAL") != nullptr)
        return;

    std::cout << CURSOR_START << CURSOR_UP << ERASE_LINE;
}

void Logger::error(const std::string& value) {
    std::raise(SIGSEGV);
}

void Logger::setTime(time_t time) {
    sTime = time;
}

const char* Logger::formatTime() {
    static char format[512];
    time_t t = sTime > 0 ? sTime : Time().sec();
    struct tm* now = localtime(&t);
    sprintf(format, "[%04d/%02d/%02d %02d:%02d:%02d]", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec);
    return format;
}