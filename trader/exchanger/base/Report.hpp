//
// Created by Вадим Проскурин on 30.09.2021.
//

#pragma once

#include "exchanger/base/OrderBase.hpp"

class Position;

struct Report {
    Report() = default;
    Report(const Position& open, const Position& close);

    // фактический заработок в Quote (уже с вычетом комиссии)
    Quantity profit;

    // количество аработанных ассетов (без учета комиссии)
    Quantity earn_base;
    Quantity earn_quote;

    // количество используем ассетов
    Quantity use_base;
    Quantity use_quote;
    Quantity use_total;

    // сумарный оборот ассетов
    Quantity volume_base;
    Quantity volume_quote;

    // количество угаданных открытий позиции
    int success = 0;
    // общее количество позиций
    int positions = 0;

    void add(const Report& report);
    Change apy() const;
};