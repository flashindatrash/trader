//
// Created by Вадим Проскурин on 29.08.2021.
//

#pragma once

#include "Defines.hpp"

class OrderBase;

NS_BEGIN
class Positions;
class Settings;
class Context;

class Status {
public: // static
    static void setTitle(const Symbol& symbol);
    static void update(Positions& positions, const Settings& settings, const Context& context);
    static void printOrder(const OrderBase& order, const std::string& type);
    static void printProfit(Quantity profit, Quantity profits, Quantity losses);
    static void printBalance(const Symbol& symbol);
};
NS_END