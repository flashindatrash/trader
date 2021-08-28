//
// Created by Вадим Проскурин on 28.08.2021.
//

#pragma once


#include "Defines.hpp"

NS_BEGIN
class Positions;
class Statistics;

class Migrator {
public: // static
    static void migrate(Positions* positions, Statistics* statistics, const Symbol& symbol, bool test);
};
NS_END