//
// Created by Вадим Проскурин on 17.07.2022.
//

#pragma once

#include "exchanger/base/Asset.hpp"
#include "exchanger/base/Wallet.hpp"

enum StakingProduct : unsigned int {
    Unknown,
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
    Decimal personal;
    Decimal minimum;
};

struct StakingRequest : public WalletRequest {
    std::string projectId;
    Decimal amount;
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
    const Decimal& minimum() const;
    Decimal staked();
    const Decimal& left();

    void updateQuota(Decimal quota);

protected: // vars
    StakingProduct _product = Unknown;
    StakingDetail _detail;
    StakingQuota _quota;
    Decimal _left;
};


