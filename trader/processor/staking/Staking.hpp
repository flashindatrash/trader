//
// Created by Вадим Проскурин on 05.04.2022.
//

#pragma once

#include "processor/Processor.hpp"

class Position;

namespace staking {
class Algorithm;

class Staking : public Processor {
public: // methods
    Staking() = default;
    ~Staking() override;

    bool init(const Settings& settings) override;
    bool isRunning() const override;

protected: // methods
    void tick(time_t ms);

protected: // vars
    Algorithm* _algorithm = nullptr;
};

}

