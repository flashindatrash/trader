#include "Strategy.hpp"
#include "Config.hpp"
#include <global.hpp>
#include "old/OldStrategy.hpp"
#include "scalping/ScalpingStrategy.hpp"

Strategy* Strategy::create(const core::Config& config) {
    std::string type = config.asString("STRATEGY");

    Strategy* strategy = nullptr;

    if (type == "scalping")
        strategy = new strategy::scalping::ScalpingStrategy();

    if (strategy == nullptr)
        return nullptr;

    if (not strategy->init(config))
        SAFE_DELETE(strategy);

    return strategy;
}
