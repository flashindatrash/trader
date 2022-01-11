//
// Created by Вадим Проскурин on 11.01.2022.
//

#pragma once

#include "base/BaseStrategy.hpp"

class Symbol;
class Position;

namespace listing {
class Strategy : public BaseStrategy {
public: // methods
    Strategy() = default;

    bool init(const core::Config &config) override;
    bool isRunning() const override;

protected: // methods

    void tick(time_t ms);

    void tryOpen(const Symbol& symbol);
    void tryClose();

protected: // vars
    core::Config _config;
    Position* _position = nullptr;

};
}

