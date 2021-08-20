#include "PairStrategy.hpp"
#include <global.hpp>
#include "Logger.hpp"
#include "Config.hpp"
#include "Settings.hpp"
#include "Runner.hpp"

NS_USE

PairStrategy::~PairStrategy() {
    SAFE_DELETE(_runner);
}

bool PairStrategy::init(const core::Config& config) {
    Settings settings(config);
    if (not settings.isValid())
        return false;

    _runner = Runner::create(settings);
    _runner->onTick.connect(std::bind(&PairStrategy::tick, this, std::placeholders::_1));
    _runner->run();
    return true;
}

bool PairStrategy::isRunning() const {
    return _runner && _runner->isRunning();
}

void PairStrategy::tick(time_t now) {
    Logger::info("%d t", now);
}
