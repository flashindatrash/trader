//
// Created by Вадим Проскурин on 05.04.2022.
//

#include "Staking.hpp"

using namespace staking;

bool Staking::init(const Settings& settings) {
    _settings = settings;
    return true;
}

bool Staking::isRunning() const {
    return false;
}