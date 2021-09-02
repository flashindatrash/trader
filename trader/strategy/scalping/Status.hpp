//
// Created by Вадим Проскурин on 29.08.2021.
//

#pragma once

#include "Defines.hpp"

class OrderBase;

NS_BEGIN
class Positions;
class Settings;

class Status {
public: // static
    static void setTitle(const Symbol& symbol, Price current);
    static void printTimeline(Positions& positions, Price current, const Settings& settings);
    static void printOrder(const OrderBase& order, const std::string& type);
    static void addProfit(Quantity profit, const Symbol& symbol);
};
NS_END