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

bool Strategy::init(const core::Config& config) {
    _config = config;

    Time().onTick.connect(std::bind(&Strategy::tick, this, std::placeholders::_1));
    return true;
}

bool Strategy::isRunning() const {
    return true;
}

void Strategy::tick(time_t ms) {
   update();
   execute();
}

void Strategy::update() {
    if (not _algorithms.empty())
        return;

    SymbolUpdater symbols;
    if (not symbols.request()) {
        Logger::info(util::format("Failed to get listed symbols"));
        return;
    }

    for (const Symbol &symbol: symbols.vector()) {
        if (Algorithm *algorithm = Algorithm::create(_config)) {
            if (not algorithm->init(symbol)) {
                Logger::info(util::format("Failed to init algorithm %s", symbol.c_str()));
                delete algorithm;
                continue;
            }

            Logger::info(util::format("Create algorithm"));
            _algorithms.push_back(algorithm);
        }
    }
}

void Strategy::execute() {
    if (_algorithms.empty())
        return;

    for (auto it = _algorithms.begin(); it != _algorithms.end();) {
        Algorithm* algorithm = *it;
        if (algorithm->execute())
            it = _algorithms.erase(it);
        else
            ++it;
    }
}