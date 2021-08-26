#pragma once

#include <stdlib.h>
#include <iostream>
#include <sys/time.h>
#include <csignal>
#include <cstdarg>
#include "Time.hpp"

//the following are UBUNTU/LINUX, and MacOS ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */
//escape chars
#define ESCSTART    "\033]0;"
#define ESCEND      "\007"


class Logger {
public: //
    static void info(const char* fmt, ...) {
        va_list arg;

        char new_fmt[1024];

        time_t t = Time().sec();
        struct tm* now = localtime(&t);

        sprintf(new_fmt, "[%04d/%02d/%02d %02d:%02d:%02d] T: %s\n", now->tm_year + 1900, now->tm_mon + 1, now->tm_mday, now->tm_hour, now->tm_min, now->tm_sec, fmt);

        va_start(arg, fmt);

        vfprintf(stdout, new_fmt, arg);
        fflush(stdout);

        va_end (arg);
    }

    static void title(const char* fmt, ...) {
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

    static void error(const char* msg) {
        info("Error! %s", msg);
        // std::raise(SIGSEGV);
    }
};
