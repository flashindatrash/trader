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

    // фактический заработок в Quote (уже с вычетом комиссии)
    Quantity profit = 0.0;
    // процентное соотношение заработка от депозита
    Change change = 0.0;

    // количество аработанных ассетов (без учета комиссии)
    Quantity earn_base = 0.0;
    Quantity earn_quote = 0.0;

    // количество используем ассетов
    Quantity use_base = 0.0;
    Quantity use_quote = 0.0;

    // сумарный оборот ассетов
    Quantity volume_base = 0.0;
    Quantity volume_quote = 0.0;

    // количество угаданных открытий позиции
    int success = 0;
    // общее количество позиций
    int positions = 0;

    void add(const Report& report);
};
NS_END