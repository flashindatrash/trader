//
// Created by Вадим Проскурин on 29.08.2021.
//

#pragma once

#include "Defines.hpp"

class OrderBase;

NS_BEGIN
class Status {
public: // static
    static void setTitle(const Symbol& symbol);
    static void printOrder(const OrderBase& order, const std::string& type);
    static void addProfit(Quantity profit);
};
NS_END