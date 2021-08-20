#include "Strategy.hpp"
#include "Config.hpp"
#include <global.hpp>
#include "scalping/ScalpingStrategy.hpp"
#include "pair/PairStrategy.hpp"

Strategy* Strategy::create(const core::Config& config) {
    std::string type = config.getAsString("STRATEGY");

    Strategy* strategy = nullptr;

    if (type == "scalping")
        strategy = new scalping::ScalpingStrategy();
    else if (type == "pair")
        strategy = new strategy::pair::PairStrategy();

    if (strategy == nullptr)
        return nullptr;

    if (not strategy->init(config))
        SAFE_DELETE(strategy);

    return strategy;
}
