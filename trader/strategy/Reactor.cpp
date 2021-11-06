//
// Created by Вадим Проскурин on 06.11.2021.
//

#include "Reactor.hpp"

NS_USE

Reactor* Reactor::create() {
    auto* reactor = new Reactor();
    return reactor;
}

bool Reactor::init() {
    return true;
}