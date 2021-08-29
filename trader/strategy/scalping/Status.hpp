//
// Created by Вадим Проскурин on 29.08.2021.
//


#pragma once

#include "Defines.hpp"

class OrderBase;

NS_BEGIN
class Status {
public: // static
    static Status* create(const Symbol& symbol);

    static void addOrder(const OrderBase& order, std::string type);

public: // methods
    void update();

protected: // methods
    Status(const Symbol& symbol);

protected: // vars
    Symbol _symbol;
};
NS_END