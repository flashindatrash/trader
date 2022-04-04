#include "Trader.hpp"
#include "Runner.hpp"
#include "Algorithm.hpp"
#include "Listener.hpp"
#include "Reactor.hpp"

using namespace trader;

Trader::~Trader() {
    delete _runner;
    delete _algorithm;
    delete _listener;
    delete _reactor;

    _runner = nullptr;
    _algorithm = nullptr;
    _listener = nullptr;
    _reactor = nullptr;
}

bool Trader::init(const Settings& settings) {
    _settings = settings;

    // check settings
    if (_settings.script().empty())
        return false;

    if (_settings.symbol().empty())
        return false;

    if (_settings.profitRatio() < 0.0 || _settings.profitRatio() > 1.0)
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

    // create & start runner
    _runner = Runner::create();
    _runner->setCallback(std::bind(&Trader::execute, this, std::placeholders::_1));

    if (not _runner->start(_settings)) {
        delete _runner;
        _runner = nullptr;
        _algorithm->stop();
    }

    return true;
}

void Trader::execute(void*) {
    _algorithm->execute();
    _reactor->execute();
}

bool Trader::isRunning() const {
    return _runner != nullptr;
}