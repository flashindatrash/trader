#include "ScalpingStrategy.hpp"
#include <global.hpp>
#include "Config.hpp"
#include "Settings.hpp"
#include "Runner.hpp"
#include "Algorithm.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"

NS_USE

ScalpingStrategy::~ScalpingStrategy() {
    SAFE_DELETE(_runner);
    SAFE_DELETE(_algorithm);
}

bool ScalpingStrategy::init(const core::Config& config) {
    Settings settings(config);
    if (not settings.isValid())
        return false;

    _algorithm = Algorithm::create(settings);
    if (not _algorithm->init())
        return false;

    _runner = Runner::create();
    _runner->setCallback(std::bind(&Algorithm::execute, _algorithm, std::placeholders::_1));
    if (not _runner->init(settings))
        return false;

    Exchanger().listenCharts(settings.symbol, ChartInterval::m5);
    return true;
}

bool ScalpingStrategy::isRunning() const {
    return _runner && _runner->isRunning();
}
