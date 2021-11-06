//
// Created by Вадим Проскурин on 06.11.2021.
//

#include "Listener.hpp"

#include <utility>
#include "Algorithm.hpp"
#include "Terminal.hpp"
#include "Position.hpp"
#include "Events.hpp"
#include "Statistics.hpp"

NS_USE

Listener* Listener::create(const Settings& settings) {
    auto* listener = new Listener(settings);
    return listener;
}

Listener::Listener(Settings settings)
    : _settings(std::move(settings))
{
}

Listener::~Listener() {
    if (_statistics != nullptr) {
        delete _statistics;
        _statistics = nullptr;
    }

    if (_events != nullptr) {
        delete _events;
        _events = nullptr;
    }
}

bool Listener::init(Algorithm& algorithm) {
    algorithm.onOpen.connect(std::bind(&Listener::handleOpen, this, std::placeholders::_1));
    algorithm.onAverage.connect(std::bind(&Listener::handleAverage, this, std::placeholders::_1));
    algorithm.onClose.connect(std::bind(&Listener::handleClose, this, std::placeholders::_1));
    algorithm.onStop.connect(std::bind(&Listener::handleStop, this, std::placeholders::_1));

    Terminal::setTitle(_settings.symbol);

    _statistics = Statistics::create(_settings.uniqId() + ":stats");
    _events = Events::create(_settings.username + ":events");

    if (_settings.isRelease()) {
        _events->send("%s start", _settings.symbol.c_str());
    }

    return true;
}

void Listener::update(const Position& position, const Context& context) {
    if (position.has() && not _settings.isBackTest()) {
        Terminal::update(position, context);
    }
}

void Listener::handleOpen(const Position& position) {
    Terminal::printOrder(position, ">");

    if (_settings.isRelease()) {
        _events->send("%s %f %s for %f", position.side() == Buy ? "buy" : "sell", position.baseQuantity(), position.symbol().baseAsset().c_str(), position.price());
    }
}

void Listener::handleAverage(const Position& position) {
    Terminal::printOrder(position, ">");

    if (_settings.isRelease()) {
        _events->send("%s %f %s for %f", position.side() == Buy ? "buy" : "sell", position.baseQuantity(), position.symbol().baseAsset().c_str(), position.price());
    }
}

void Listener::handleClose(const Report& report) {
    Terminal::printProfit(report, _settings.symbol.quoteAsset());

    _report.add(report);

    // сохраняем статистику закрытия сделки
    _statistics->report(report);
    if (_settings.isRelease()) {
        _statistics->save();
        _events->send("profit: %f %s", report.profit, _settings.symbol.quoteAsset().c_str());
    }
}

void Listener::handleStop(void*) {
    Terminal::printReport(_report, _settings.symbol);
}