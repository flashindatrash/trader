//
// Created by Вадим Проскурин on 05.04.2022.
//

#include <binacpp.h>
#include <algorithm>
#include <core/Logger.hpp>
#include "Staking.hpp"
#include "core/Time.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/StakingWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"

using namespace staking;

#include "exchanger/base/Decimal.hpp"
bool Staking::init(const Settings& settings) {
    _settings = settings;
    Time().onTick.connect(std::bind(&Staking::tick, this, std::placeholders::_1));
    return true;
}

bool Staking::isRunning() const {
    return true;
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
    if (ms > stakeProject + Timer::sMinute * 10) {
        StakingRequest request;

        // find suitable staking project
        if (StakingWrapper* staking = findStaking(request.mask(StakingRequest::RedeemSavings))) {
            request.projectId = staking->id();
            request.amount = staking->asset().balance();

            // add flexible balance
            if (request.mask(StakingRequest::RedeemSavings))
                request.amount += Asset("LD" + staking->asset().id()).balance();

            // not more than quota
            request.amount = std::min(request.amount, staking->quota());

            if (Exchanger().stake(request)) {
                Logger::info(util::format("Staked %s %s with %d%% APY on %d days", request.amount.c_str(), staking->asset().c_str(), int(staking->apy() * 100), staking->duration()));
            } else {
                Logger::error(util::format("Failed to stake %s %s with %d%% APY on %d days", request.amount.c_str(), staking->asset().c_str(), int(staking->apy() * 100), staking->duration()));
            }
        }

        stakeProject = ms;
    }
}

StakingWrapper* Staking::findStaking(bool use_flexible_balance) const {
    // loop over balances (spot + flexible staking)
    for (auto& pair : Exchanger().balances()) {
        if (pair.second->get() <= 0)
            continue;

        std::string asset = pair.first;
        if (asset.rfind("LD", 0) == 0) {
            asset = asset.substr(2);
            if (Exchanger().balance(asset) == nullptr) {
                Logger::info(util::format("Unknown asset %s (flexible %s)", asset.c_str(), pair.first.c_str()));
                continue;
            }
        }

        // todo: skip BNB
        if (asset.rfind("BNB", 0) == 0)
            continue;

        // find all projects by staking asset
        std::vector<StakingWrapper*> stakings = findStaking(asset);
        std::sort(stakings.begin(), stakings.end(), [](StakingWrapper* lhs, StakingWrapper* rhs) {
            return lhs->apy() > rhs->apy();
        });

        Quantity quantity = Asset(asset).balance();
        if (use_flexible_balance)
            quantity += Asset("LD" + asset).balance();

        for (StakingWrapper* staking : stakings) {
            // check enough for minimum
            if (quantity < staking->minimum())
                continue;

            // check enough for quota
            if (staking->quota() < staking->minimum())
                continue;

            return staking;
        }
    }

    return nullptr;
}

std::vector<StakingWrapper*> Staking::findStaking(const Asset& asset) {
    std::vector<StakingWrapper*> result;

    // loop over stakings
    for (auto& staking : Exchanger().stakings()) {
        if (staking.second->asset().id() == asset.id())
            result.push_back(staking.second);
    }

    return result;
}
