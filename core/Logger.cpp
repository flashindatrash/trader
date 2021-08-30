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
#include "Time.hpp"

const char* Logger::_log_file = "traderbot";

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
    va_list arg;
    va_start(arg, fmt);
    vfprintf(stdout, format(fmt), arg);
    fflush(stdout);
    va_end (arg);
}

void Logger::trace(const char* fmt, ...) {
    static FILE* file = nullptr;
    if (file == nullptr) {
        std::string path = "/tmp/";
        path.append(_log_file);
        path.append(".log");
        file = fopen(path.c_str(), "wa");
        if (file) {
            info("log file in %s", path.c_str());
        } else {
            info("failed to open log file");
        }
    }

    if (file == nullptr)
        return;

    va_list arg;
    va_start(arg, fmt);
    vfprintf(file, format(fmt), arg);
    fflush(file);
    va_end (arg);
}

void Logger::error(const char* msg) {
    info("Error! %s", msg);
    // std::raise(SIGSEGV);
}

void Logger::setLogfile(const char* filename) {
    _log_file = filename;
}

const char* Logger::format(const char* fmt) {
    static char new_fmt[1024];
    time_t t = Time().sec();
    struct tm* now = localtime(&t);
    sprintf(new_fmt, "[%04d/%02d/%02d %02d:%02d:%02d] T: %s\n", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, fmt);
    return new_fmt;
}