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
    Settings settings(config);
    if (not settings.isValid())
        return false;

    // create algorithm
    _algorithm = Algorithm::create(settings);
    if (not _algorithm->init())
        return false;

    // create listener
    _listener = Listener::create(settings);
    if (not _listener->init(*_algorithm))
        return false;

    // create reactor
    _reactor = Reactor::create(settings);
    if (not _reactor->init())
        return false;

    // load chart
    time_t now = Time().ms();
    ChartRequest request;
    request.interval = ChartInterval::m5;
    for (int i = settings.isBackTest() ? 4 : 1; i > 0; --i) {
        request.time_start = now - Timer::sDay * i;
        request.time_end = now - Timer::sDay * (i - 1);
        if (not Exchanger().loadCharts(settings.symbol, request))
            return false;
    }

    // start listen chart
    if (not settings.isBackTest()) {
        Exchanger().listenCharts(settings.symbol, ChartInterval::m5);
        Exchanger().listenTickers(settings.symbol);
    }

    // create & start runner
    _runner = Runner::create();
    _runner->setCallback(std::bind(&Strategy::execute, this, std::placeholders::_1));
    _runner->start(settings);

    // dispatch start/stop
    if (isRunning())
        _algorithm->start();
    else
        _algorithm->stop();
    return true;
}

void Strategy::execute(const Context& context) {
    const Position& position = _algorithm->execute(context);

    _listener->update(position, context);
    _reactor->process(*_algorithm, context, *_listener);
}

bool Strategy::isRunning() const {
    return _runner && _runner->isActive();
}