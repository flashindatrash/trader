//
// Created by Вадим Проскурин on 30.08.2021.
//

#include "Logger.hpp"
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <ctime>
#include <csignal>
#include <utility>
#include "Time.hpp"

time_t Logger::sTime = 0;

void Logger::title(const std::string& value) {
    std::cout << GREEN << value << RESET << std::endl;
}

void Logger::info(const std::string& value) {
    std::cout << formatTime() << ": " << value << std::endl;
}

void Logger::status(const std::string& value) {
    static time_t last = 0;

    auto now = std::time(0);
    if (now - last < 60)
        return;
    else last = now;

    std::cout << value << std::endl;
}

void Logger::error(const std::string& value) {
    info(value);
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