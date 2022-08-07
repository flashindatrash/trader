//
// Created by Вадим Проскурин on 06.08.2022.
//

#include "Algorithm.hpp"
#include "core/Logger.hpp"
#include "base/Settings.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Position.hpp"
#include "exchanger/wrapper/StakingWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"

using namespace staking;

Algorithm* Algorithm::create(const Settings& settings) {
    auto algorithm = new Algorithm(settings);
    return algorithm;
}

Algorithm::Algorithm(const Settings& settings)
    : _settings(settings)
{
}

Algorithm::~Algorithm() {
    for (Position* position : _positions)
        delete position;

    _positions.clear();
}

bool Algorithm::init() {
    return true;
}

bool Algorithm::tryStake() {
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
            Logger::info(util::format("%sStaked %s %s with %d%% APY on %d days%s", GREEN, request.amount.c_str(), staking->asset().c_str(), int(staking->apy() * 100), staking->duration(), RESET));
            return true;
        } else {
            Logger::info(util::format("%sFailed to stake %s %s with %d%% APY on %d days%s", RED, request.amount.c_str(), staking->asset().c_str(), int(staking->apy() * 100), staking->duration(), RESET));
        }
    }

    return false;
}

StakingWrapper* Algorithm::findStaking(bool use_flexible_balance) const {
    // loop over balances (spot + flexible staking)
    for (auto& pair : Exchanger().balances()) {
        if (pair.second->get() <= 0)
            continue;

        std::string ticker = pair.first;
        if (ticker.rfind("LD", 0) == 0) {
            ticker = ticker.substr(2);
            if (Exchanger().balance(ticker) == nullptr) {
                Logger::info(util::format("Unknown asset %s (flexible %s)", ticker.c_str(), pair.first.c_str()));
                continue;
            }
        }

        Asset asset(ticker);

        // skip bnb and usd
        if (asset.id() == "BNB" || asset.isUSD())
            continue;

        // find all projects by staking asset
        std::vector<StakingWrapper*> stakings = findStaking(asset);
        std::sort(stakings.begin(), stakings.end(), [](StakingWrapper* lhs, StakingWrapper* rhs) {
            return lhs->apy() > rhs->apy();
        });

        Quantity quantity = asset.balance();
        if (use_flexible_balance)
            quantity += Asset("LD" + ticker).balance();

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

std::vector<StakingWrapper*> Algorithm::findStaking(const Asset& asset) {
    std::vector<StakingWrapper*> result;

    // loop over stakings
    for (auto& staking : Exchanger().stakings()) {
        const StakingProduct& product = staking.second->product();
        const Asset& staking_asset = staking.second->asset();

        if (product == Unknown)
            continue;

        if ((staking_asset.id() == "ETH" || staking_asset.id() == "BTC") && product == DeFiFlexible)
            continue;

        if (staking_asset.id() == asset.id())
            result.push_back(staking.second);
    }

    return result;
}