//
// Created by Вадим Проскурин on 17.07.2022.
//

#include "StakingWrapper.hpp"
#include "exchanger/Exchanger.hpp"
#include <utility>

StakingWrapper* StakingWrapper::create() {
    auto* wrapper = new StakingWrapper();
    return wrapper;
}

void StakingWrapper::set(StakingProduct product, StakingDetail detail, StakingQuota quota) {
    _product = product;
    _detail = std::move(detail);
    _quota = quota;

    // after setting project we need actualize quota
    _left = -1;
}

const StakingProduct& StakingWrapper::product() const {
    return _product;
}

const Asset& StakingWrapper::asset() const {
    return _detail.asset_stake;
}

const double& StakingWrapper::apy() const {
    return _detail.apy;
}

const uint16_t& StakingWrapper::duration() const {
    return _detail.duration;
}

const Decimal& StakingWrapper::minimum() const {
    return _quota.minimum;
}

Decimal StakingWrapper::staked() {
    const Decimal& quota = left();
    if (quota < Decimal::Zero)
        return Decimal::Zero;

    return _quota.personal - quota;
}

const Decimal& StakingWrapper::left() {
    if (_left < Decimal::Zero)
        Exchanger().updateStaking(id());

    return _left;
}

void StakingWrapper::updateQuota(Decimal quota) {
    _left = quota;
}