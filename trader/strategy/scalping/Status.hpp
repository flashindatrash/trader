//
// Created by Вадим Проскурин on 29.08.2021.
//


#pragma once

#include "Defines.hpp"
#include "Algorithm.hpp"

class OrderBase;

NS_BEGIN
class Status {
public: // static
    static Status* create(const Symbol& symbol);

    static void addOrder(const OrderBase& order, const std::string& type);
    static void addProfit(Quantity profit);

protected: // static
    static std::string result_str(Algorithm::Result result);

public: // methods
    void update(const Context& context, Algorithm::Result close, Algorithm::Result open);

protected: // methods
    explicit Status(Symbol symbol);

protected: // vars
    Symbol _symbol;
};
NS_END