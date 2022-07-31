//
// Created by Вадим Проскурин on 05.04.2022.
//

#pragma once

#include "processor/Processor.hpp"

class StakingWrapper;

namespace staking {

class Staking : public Processor {
public: // methods
    Staking() = default;

    bool init(const Settings& settings) override;
    bool isRunning() const override;

protected: // methods
    void tick(time_t ms);

    StakingWrapper* findStaking(bool use_flexible_balance) const;
    static std::vector<StakingWrapper*> findStaking(const Asset& asset) ;
};

}

