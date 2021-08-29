//
// Created by Вадим Проскурин on 29.08.2021.
//


#pragma once

#include "Defines.hpp"

NS_BEGIN
class Status {
public: // static
    static Status* create(const Symbol& symbol);

public: // methods
    void setSymbol(const Symbol& symbol);

protected: // methods
    Status() = default;
};
NS_END