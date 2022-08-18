//
// Created by Вадим Проскурин on 06.08.2022.
//

#include "Algorithm.hpp"
#include <algorithm>
#include "core/Logger.hpp"
#include "base/Settings.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/base/Position.hpp"
#include "exchanger/base/OrderCreator.hpp"
#include "exchanger/wrapper/StakingWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"

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
    for (const auto& it : _positions)
        delete it.second;

    _positions.clear();
}

bool Algorithm::init() {
    return true;
}

bool Algorithm::execute() {
    StakingRequest request;

    // find suitable staking project
    if (StakingWrapper* staking = findStaking(request.mask(StakingRequest::RedeemSavings))) {
        if (tryClose(staking->asset()))
            return true;

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

bool Algorithm::tryClose(const Asset& asset) {
    Position& position = findPosition(asset);
    if (not position.has() || position.side() != Buy)
        return false;

    const Decimal current = position.symbol().price(position.revert());
    const Decimal profit = position.profit(current);
    if (profit < 0LL)
        return false;

    Decimal quantity = position.baseQuantity();
    Decimal balance = asset.balance() + Asset("LD" + asset.id()).balance();

    // создадим реквест
    OrderRequest request;
    request.symbol = position.symbol();
    request.side = position.revert();
    request.quantity = std::min(quantity, balance);

    // создадим заказ
    Position close;
    if (not OrderCreator::create(request, close, _settings.isRelease()))
        return false;

    position.merge(close);
    if (position.baseQuantity() > 0LL)
        position.save(_settings.isRelease());
    else
        position.remove(_settings.isRelease());

    Logger::info(util::format("Sell %s %s for %s", position.baseQuantity().c_str(), position.symbol().baseAsset().c_str(), position.price().c_str()));
    return true;
}

Position& Algorithm::findPosition(const Asset& asset) {
    static const Asset quote = Asset::USDT;

    auto it = _positions.find(asset);
    if (it == _positions.end()) {
        Position* position = Position::create(_settings.username(), Symbol(asset, quote));
        _positions.insert(std::make_pair(asset, position));
        return *position;
    } else
        return *it->second;
}

StakingWrapper* Algorithm::findStaking(bool use_flexible_balance) const {
    // loop over balances (spot + flexible staking)
    for (auto& pair : Exchanger().balances()) {
        if (pair.second->get() <= 0LL)
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

        // skip bnb and usdt
        if (asset.id() == "BNB" || asset.id() == "USDT")
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