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

const double& StakingWrapper::minimum() const {
    return _quota.minimum;
}

const double& StakingWrapper::quota() {
    static double error = 0.0;
    if (not _personal_quota_actualized) {
        if (not Exchanger().updateStaking(id()))
            return error;
        _personal_quota_actualized = true;
    }

    return _quota.personal;
}

void StakingWrapper::updateQuota(double quota) {
    _quota.personal = quota;
}