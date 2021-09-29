//
// Created by Вадим Проскурин on 30.09.2021.
//

#pragma once

#include "Defines.hpp"

NS_BEGIN
class Position;

struct Report {
    Quantity profit = 0.0;
    Quantity use_base = 0.0;
    Quantity use_quote = 0.0;

    Quantity add(const Position& open, const Position& close);
};
NS_END