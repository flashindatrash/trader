#pragma once

#include "strategy/Strategy.hpp"

namespace pair {

class PairStrategy : public Strategy {
public: // methods
    PairStrategy() = default;

    bool init(const core::Config& config) override;

protected: // methods

};

}
