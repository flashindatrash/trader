//
// Created by Вадим Проскурин on 05.04.2022.
//

#include "Staking.hpp"
#include "Algorithm.hpp"

#include "core/Time.hpp"
#include "exchanger/Exchanger.hpp"

using namespace staking;

Staking::~Staking() {
    delete _algorithm;
    _algorithm = nullptr;
}

bool Staking::init(const Settings& settings) {
    _settings = settings;

    // create algorithm
    _algorithm = Algorithm::create(_settings);
    if (not _algorithm->init())
        return false;

    Time().onTick.connect(std::bind(&Staking::tick, this, std::placeholders::_1));
    return true;
}

bool Staking::isRunning() const {
    return _algorithm != nullptr;
}

void Staking::tick(time_t ms) {
    // update project list every hour
    static time_t updateProjects = 0;
    if (ms > updateProjects + Timer::sHour) {
        Exchanger().loadStakings();
        updateProjects = ms;
    }

    // stake every 10 minute
    static time_t stakeProject = 0;
    if (ms > stakeProject + Timer::sMinute * 5) {
        _algorithm->execute();
        stakeProject = ms;
    }
}
