//
// Created by Вадим Проскурин on 11.01.2022.
//

#include "Listing.hpp"
#include "Algorithm.hpp"
#include "SymbolUpdater.hpp"
#include "core/Logger.hpp"
#include "core/Time.hpp"

using namespace listing;

Listing::~Listing() {
    for (Algorithm* algorithm : _algorithms)
        delete algorithm;

    _algorithms.clear();
}

bool Listing::init(const Settings& settings) {
    _settings = settings;

    Time().onTick.connect(std::bind(&Listing::tick, this, std::placeholders::_1));
    return true;
}

bool Listing::isRunning() const {
    return _running;
}

void Listing::tick(time_t ms) {
   update();
   execute();
}

bool Listing::add(const Symbol& symbol) {
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

void Listing::update() {
    if (not _algorithms.empty())
        return;

    // for test: check if already exist pair
    if (_settings.isDevelop() && not _settings.symbol().empty() && _settings.symbol().exists()) {
        add(_settings.symbol());
        return;
    }

    SymbolUpdater symbols;
    if (not symbols.request())
        return;

    for (const Symbol& symbol: symbols.vector()) {
        // interested symbol from arguments
        if (not _settings.symbol().empty()) {
            if (symbol.id() == _settings.symbol().id()) {
                add(symbol);
                break;
            } else continue;
        }

        // interested USDT pairs
        if (symbol.quoteAsset().id() == Asset::USDT.id())
            add(symbol);
    }
}

void Listing::execute() {
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