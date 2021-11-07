//
// Created by Вадим Проскурин on 30.09.2021.
//

#pragma once

#include "Defines.hpp"

NS_BEGIN
class Position;

struct Report {
    Report() = default;
    Report(const Position& open, const Position& close);

    Quantity profit = 0.0;
    Quantity earn_base = 0.0;
    Quantity earn_quote = 0.0;

    Change change = 0.0;
    Quantity use_base = 0.0;
    Quantity use_quote = 0.0;

    int success = 0;
    int positions = 0;

    void add(const Report& report);
};
NS_END