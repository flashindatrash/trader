//
// Created by Вадим Проскурин on 28.08.2021.
//

#pragma once


#include "Defines.hpp"

NS_BEGIN
class Positions;

class Migrator {
public: // static
    static void migrate(Positions* positions, const Symbol& symbol);
};
NS_END