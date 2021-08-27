#pragma once

#include "Defines.hpp"
#include "strategy/Strategy.hpp"

NS_BEGIN
class Runner;
class Algorithm;

class ScalpingStrategy : public Strategy {
public: // methods
    ScalpingStrategy() = default;
    ~ScalpingStrategy() override;

    bool init(const core::Config& config) override;
    bool isRunning() const override;

protected: // methods
    Runner* _runner = nullptr;
    Algorithm* _algorithm = nullptr;
};
NS_END


