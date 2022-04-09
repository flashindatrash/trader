//
// Created by Вадим Проскурин on 05.04.2022.
//

#include "Staking.hpp"
#include "core/Time.hpp"
#include "exchanger/Exchanger.hpp"

using namespace staking;

bool Staking::init(const Settings& settings) {
    _settings = settings;
    Time().onTick.connect(std::bind(&Staking::tick, this, std::placeholders::_1));
    return true;
}

bool Staking::isRunning() const {
    return true;
}

void Staking::tick(time_t ms) {
    static time_t first = 0;
    static bool pause = false;
    if (first == 0) {
        first = ms;
        if (not pause)
            Exchanger().listenTickers("BTCUSDT");
    }

    if (ms > first + Timer::sSecond * 10) {
        Exchanger().unlistenTickers("BTCUSDT");
        first = 0;
        pause = !pause;
    }
}