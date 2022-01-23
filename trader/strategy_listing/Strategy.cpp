//
// Created by Вадим Проскурин on 11.01.2022.
//

#include "Strategy.hpp"
#include "Algorithm.hpp"
#include "SymbolUpdater.hpp"
#include "core/Logger.hpp"
#include "core/Time.hpp"

using namespace listing;

Strategy::~Strategy() {
    for (Algorithm* algorithm : _algorithms)
        delete algorithm;

    _algorithms.clear();
}

bool Strategy::init(const Settings& settings) {
    _settings = settings;

    Time().onTick.connect(std::bind(&Strategy::tick, this, std::placeholders::_1));
    return true;
}

bool Strategy::isRunning() const {
    return _running;
}

void Strategy::tick(time_t ms) {
   update();
   execute();
}

bool Strategy::add(const Symbol& symbol) {
    Algorithm* algorithm = Algorithm::create(_settings);
    if (algorithm == nullptr)
        return false;

    if (not algorithm->init(symbol)) {
        delete algorithm;
        return false;
    }

    Logger::info(util::format("Create algorithm"));
    _algorithms.push_back(algorithm);
    return true;
}

void Strategy::update() {
    if (not _algorithms.empty())
        return;

    SymbolUpdater symbols;
    if (not symbols.request()) {
        Logger::info(util::format("Failed to get listed symbols"));
        return;
    }

    for (const Symbol& symbol: symbols.vector()) {
        bool interested = symbol.id() == _settings.symbol().id();

        // interested symbol from arguments
        if (not _settings.symbol().empty() && not interested)
            continue;

        // interested USDT pairs
        interested |= symbol.quoteAsset().id() == Asset::USDT.id();

        if (interested)
            add(symbol);
    }
}

void Strategy::execute() {
    if (_algorithms.empty())
        return;

    for (auto it = _algorithms.begin(); it != _algorithms.end();) {
        Algorithm* algorithm = *it;
        if (algorithm->execute()) {
            it = _algorithms.erase(it);
            delete algorithm;
        } else {
            ++it;
        }
    }

    // stop running
    if (_algorithms.empty() && not _settings.symbol().empty())
        _running = false;
}