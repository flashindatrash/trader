//
// Created by Вадим Проскурин on 28.08.2021.
//

#pragma once


#include "Defines.hpp"

NS_BEGIN
class Position;
class Statistics;

class Migrator {
public: // static
    static bool migrate(Position* position, Statistics* statistics, const Symbol& symbol);
};
NS_END