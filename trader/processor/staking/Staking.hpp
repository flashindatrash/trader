//
// Created by Вадим Проскурин on 05.04.2022.
//

#pragma once

#include "processor/Processor.hpp"

namespace staking {

class Staking : public Processor {
public: // methods
    Staking() = default;

    bool init(const Settings& settings) override;
    bool isRunning() const override;
};

}

