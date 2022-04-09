//
// Created by Вадим Проскурин on 09.04.2022.
//

#pragma once
#include <string>

struct tm;

class SymbolFrequency {
public:
    static bool request();

private:
    static bool between(const std::string& now, const std::string& begin, const std::string& end);
};


