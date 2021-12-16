#include "Strategy.hpp"
#include "Time.hpp"
#include "Config.hpp"
#include "Runner.hpp"
#include "Algorithm.hpp"
#include "Listener.hpp"
#include "Reactor.hpp"
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
    delete _runner;
    delete _algorithm;
    delete _listener;
    delete _reactor;

    _runner = nullptr;
    _algorithm = nullptr;
    _listener = nullptr;
    _reactor = nullptr;
}

bool Strategy::init(const core::Config& config) {
    _settings = Settings(config);
    if (not _settings.isValid())
        return false;

    // create algorithm
    _algorithm = Algorithm::create(_settings);
    if (not _algorithm->init())
        return false;

    // create listener
    _listener = Listener::create(_settings);
    if (not _listener->init(*_algorithm))
        return false;

    // create reactor
    _reactor = Reactor::create(*_algorithm, _settings);
    if (not _reactor->init())
        return false;

    // load chart
    time_t now = Time().ms();
    ChartRequest request;
    request.interval = ChartInterval::m5;
    for (int i = _settings.isBackTest() ? 2 : 1; i > 0; --i) {
        request.time_start = now - Timer::sDay * i;
        request.time_end = now - Timer::sDay * (i - 1);
        if (not Exchanger().loadCharts(_settings.symbol, request))
            return false;
    }

    // start listen chart
    if (not _settings.isBackTest()) {
        Exchanger().listenCharts(_settings.symbol, request.interval);
        Exchanger().listenTickers(_settings.symbol);
    }

    if (_settings.isBackTest()) {
        Exchanger().balance(_settings.symbol.baseAsset())->gain(1000);
        Exchanger().balance(_settings.symbol.quoteAsset())->gain(1000);
    }

    // create & start runner
    _runner = Runner::create();
    _runner->setCallback(std::bind(&Strategy::execute, this, std::placeholders::_1));

    _algorithm->start();
    _runner->start(_settings);
    if (not isRunning())
        _algorithm->stop();

    return true;
}

void Strategy::execute(void*) {
    _algorithm->execute();
    _reactor->execute();
}

bool Strategy::isRunning() const {
    return _runner && _runner->isActive();
}