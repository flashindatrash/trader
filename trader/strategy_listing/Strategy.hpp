//
// Created by Вадим Проскурин on 11.01.2022.
//

#pragma once

#include "base/BaseStrategy.hpp"
#include <vector>

namespace listing {
class Algorithm;

class Strategy : public BaseStrategy {
public: // methods
    Strategy() = default;
    ~Strategy() override;

    bool init(const Settings& settings) override;
    bool isRunning() const override;

protected: // methods
    void tick(time_t ms);

    void update();
    void execute();

protected: // vars
    std::vector<Algorithm*> _algorithms;
};

}

