//
// Created by Вадим Проскурин on 29.08.2021.
//

#pragma once

#include "Defines.hpp"

NS_BEGIN
class Position;
class Context;
struct Report;

class Terminal {
public: // static
    static void setTitle(const Symbol& symbol);
    static void update(const Position& position, const Context& context);
    static void printOrder(const OrderBase& order, const std::string& type);
    static void printProfit(const Report& report, const Asset& asset);
    static void printReport(const Report& report, const Symbol& symbol);
};
NS_END