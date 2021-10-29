#include "Strategy.hpp"
#include <global.hpp>
#include "Time.hpp"
#include "Config.hpp"
#include "Settings.hpp"
#include "Runner.hpp"
#include "Algorithm.hpp"
#include "exchanger/Exchanger.hpp"
#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/BalanceWrapper.hpp"

NS_USE

Strategy* Strategy::create(const core::Config& config) {
    auto* strategy = new Strategy();
    strategy->init(config);
    return strategy;
}

Strategy::~Strategy() {
    SAFE_DELETE(_runner);
    SAFE_DELETE(_algorithm);
}

bool Strategy::init(const core::Config& config) {
    Settings settings(config);
    if (not settings.isValid())
        return false;

    // create algorithm
    _algorithm = Algorithm::create(settings);
    if (not _algorithm->init())
        return false;

    // load chart
    if (settings.isBackTest()) {
        time_t now = Time().ms();
        ChartRequest request;
        request.interval = ChartInterval::m5;
        for (int i = 30; i > 0; --i) {
            request.time_start = now - Timer::sDay * (i);
            request.time_end = now - Timer::sDay * (i - 1);
            Exchanger().loadCharts(settings.symbol, request);
        }
    } else {
        ChartRequest request;
        request.interval = ChartInterval::m5;
        request.time_end = Time().ms();
        request.time_start = request.time_end - Timer::sDay;
        Exchanger().loadCharts(settings.symbol, request);
    }

    // start listen chart
    if (not settings.isBackTest()) {
        Exchanger().listenCharts(settings.symbol, ChartInterval::m5);
        Exchanger().listenTickers(settings.symbol);
    }

    // add test balance
    if (settings.isBalanceUnlimited()) {
        Exchanger().balance(settings.symbol.baseAsset())->gain(10000);
        Exchanger().balance(settings.symbol.quoteAsset())->gain(10000);
    }

    // create & start runner
    _runner = Runner::create();
    _runner->setCallback(std::bind(&Algorithm::execute, _algorithm, std::placeholders::_1));
    _runner->start(settings);

    // remove test balance
    if (settings.isBalanceUnlimited()) {
        Exchanger().balance(settings.symbol.baseAsset())->spend(10000);
        Exchanger().balance(settings.symbol.quoteAsset())->spend(10000);
    }

    // print report
    if (settings.isBackTest()) {
        _algorithm->report();
    }

    return true;
}

bool Strategy::isRunning() const {
    return _runner && _runner->isActive();
}