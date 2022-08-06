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

const Decimal& StakingWrapper::quota() {
    static const Decimal zero;
    if (not _personal_quota_actualized) {
        if (not Exchanger().updateStaking(id()))
            return zero;
        _personal_quota_actualized = true;
    }

    return _quota.personal;
}

void StakingWrapper::updateQuota(Decimal quota) {
    _quota.personal = quota;
}