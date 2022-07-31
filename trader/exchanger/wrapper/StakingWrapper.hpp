//
// Created by Вадим Проскурин on 17.07.2022.
//

#pragma once

#include "exchanger/base/Asset.hpp"
#include "exchanger/base/Wallet.hpp"

enum StakingProduct : unsigned int {
	Locked,
	DeFiLocked,
	DeFiFlexible
};

struct StakingDetail {
    Asset asset_stake;
    Asset asset_reward;
    double apy = 0.0;
    uint16_t duration = 0;
};

struct StakingQuota {
    double personal = 0.0;
    double minimum = 0.0;
};

struct StakingRequest : public WalletRequest {
    std::string projectId;
    double amount = 0.0;
};

class StakingWrapper : public Identifier {
public: // static
    static StakingWrapper* create();

public: // methods
	StakingWrapper() = default;

    void set(StakingProduct product, StakingDetail detail, StakingQuota quota);

    const StakingProduct& product() const;
    const Asset& asset() const;
    const double& apy() const;
    const uint16_t& duration() const;
    const double& minimum() const;
    const double& quota();

    void updateQuota(double quota);

protected: // vars
    StakingProduct _product;
    StakingDetail _detail;
    StakingQuota _quota;
    bool _personal_quota_actualized = false;
};


