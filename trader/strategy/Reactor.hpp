//
// Created by Вадим Проскурин on 06.11.2021.
//

#pragma once

#include "Defines.hpp"

NS_BEGIN
class Reactor {
public: // static
    static Reactor* create();

public: // methods
    bool init();

protected: // methods
    Reactor() = default;

};
NS_END