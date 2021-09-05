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

    // create algorithm
    _algorithm = Algorithm::create(settings);
    if (not _algorithm->init())
        return false;

    // load last 24 chart & start listening
    if (not Exchanger().loadCharts(settings.symbol, ChartInterval::m15))
        return false;
    Exchanger().listenCharts(settings.symbol, ChartInterval::m5);

    // create & start runner
    _runner = Runner::create();
    _runner->setCallback(std::bind(&Algorithm::execute, _algorithm, std::placeholders::_1));
    if (not _runner->start(settings))
        return false;

    return true;
}

bool ScalpingStrategy::isRunning() const {
    return _runner && _runner->isRunning();
}
