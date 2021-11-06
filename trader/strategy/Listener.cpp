//
// Created by Вадим Проскурин on 06.11.2021.
//

#include "Listener.hpp"

#include <utility>
#include "Algorithm.hpp"
#include "EventFormatter.hpp"
#include "Position.hpp"
#include "Events.hpp"
#include "Statistics.hpp"
#include "Logger.hpp"

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

    Logger::title(EventFormatter::title(_settings.symbol));

    _statistics = Statistics::create(_settings.uniqId() + ":stats");
    _events = Events::create(_settings.username + ":events");
    return true;
}

void Listener::update(const Position& position, const Context& context) {
    if (position.has() && not _settings.isBackTest()) {
        std::string event = EventFormatter::update(position, context);
        Logger::status(event);
    }
}

void Listener::handleOpen(const Position& position) {
    std::string event = EventFormatter::order(position);
    Logger::info(event);

    if (_settings.isRelease())
        _events->push(event);
}

void Listener::handleAverage(const Position& position) {
    std::string event = EventFormatter::order(position);
    Logger::info(event);

    if (_settings.isRelease())
        _events->push(event);
}

void Listener::handleClose(const Report& report) {
    std::string event = EventFormatter::profit(report, _settings.symbol.quoteAsset());
    Logger::info(event);

    // добавим в общий репорт
    _report.add(report);

    // сохраняем статистику закрытия сделки
    _statistics->report(report);
    if (_settings.isRelease()) {
        _statistics->save();
        _events->push(event);
    }
}

void Listener::handleStop(void*) {
    std::string event = EventFormatter::report(_report, _settings.symbol);
    Logger::info(event);
}