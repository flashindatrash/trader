#include "ScalpingStrategy.hpp"
#include <global.hpp>
#include "Config.hpp"
#include "Settings.hpp"
#include "Runner.hpp"
#include "Algorithm.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"

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

    // set chart interval
    Exchanger().chart(settings.symbol)->setInterval(ChartInterval::m5);

    // load 24h chart
    if (not Exchanger().loadCharts(settings.symbol))
        return false;

    // start listen chart
    Exchanger().listenCharts(settings.symbol);
    Exchanger().listenTickers(settings.symbol);

    // add test balance
    if (settings.test) {
        Exchanger().balance(settings.symbol.baseAsset())->gain(10000);
        Exchanger().balance(settings.symbol.quoteAsset())->gain(10000);
    }

    // create & start runner
    _runner = Runner::create();
    _runner->setCallback(std::bind(&Algorithm::execute, _algorithm, std::placeholders::_1));
    _runner->start(settings);
    return isRunning();
}

bool ScalpingStrategy::isRunning() const {
    return _runner && _runner->isActive();
}
