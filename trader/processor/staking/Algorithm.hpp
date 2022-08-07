//
// Created by Вадим Проскурин on 06.08.2022.
//


#pragma once

#include <vector>

class Settings;
class Asset;
class Position;
class StakingWrapper;

namespace staking {

class Algorithm {
public: // static
    static Algorithm* create(const Settings& config);

public: // methods
    virtual ~Algorithm();

    bool init();
    bool tryStake();

protected: // methods
    explicit Algorithm(const Settings& config);

    StakingWrapper* findStaking(bool use_flexible_balance) const;
    static std::vector<StakingWrapper*> findStaking(const Asset& asset);

protected: // vars
    const Settings& _settings;

    std::vector<Position*> _positions;
};

}