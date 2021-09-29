//
// Created by Вадим Проскурин on 29.08.2021.
//

#pragma once

#include "Defines.hpp"

class OrderBase;

NS_BEGIN
class Position;
class Settings;
class Context;

class Terminal {
public: // static
    static void setTitle(const Symbol& symbol);
    static void update(Position& position, const Settings& settings, const Context& context);
    static void printOrder(const OrderBase& order, const std::string& type);
    static void printProfit(Quantity profit, Quantity sum);
};
NS_END