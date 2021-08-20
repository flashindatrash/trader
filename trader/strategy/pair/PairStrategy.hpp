#pragma once

#include "Defines.hpp"
#include "strategy/Strategy.hpp"

NS_BEGIN
class Runner;

class PairStrategy : public Strategy {
public: // methods
    PairStrategy() = default;
    ~PairStrategy() override;

    virtual bool init(const core::Config& config) override;
    virtual bool isRunning() const override;

protected: // methods
    void tick(time_t now);

protected: // methods
    Runner* _runner = nullptr;
};
NS_END


