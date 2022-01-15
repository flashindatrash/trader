//
// Created by Вадим Проскурин on 11.01.2022.
//

#pragma once

#include "base/BaseStrategy.hpp"

namespace listing {

class Strategy : public BaseStrategy {
public: // methods
    Strategy() = default;
    ~Strategy() override;

    bool init(const core::Config &config) override;
    bool isRunning() const override;

protected: // methods

    void tick(time_t ms);

    std::string username() const;

protected: // vars
    core::Config _config;

};
}

