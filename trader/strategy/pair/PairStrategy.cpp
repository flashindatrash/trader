#include "PairStrategy.hpp"
#include <global.hpp>
#include "Config.hpp"
#include "Settings.hpp"
#include "Runner.hpp"
#include "Algorithm.hpp"

NS_USE

PairStrategy::~PairStrategy() {
    SAFE_DELETE(_runner);
    SAFE_DELETE(_algorithm);
}

bool PairStrategy::init(const core::Config& config) {
    Settings settings(config);
    if (not settings.isValid())
        return false;

    _algorithm = Algorithm::create(settings);
    if (not _algorithm->init())
        return false;

    _runner = Runner::create();
    _runner->setCallback(std::bind(&Algorithm::execute, _algorithm, std::placeholders::_1));
    return _runner->init(settings);
}

bool PairStrategy::isRunning() const {
    return _runner && _runner->isRunning();
}
