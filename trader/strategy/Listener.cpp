//
// Created by Вадим Проскурин on 06.11.2021.
//

#include "Listener.hpp"

#include <utility>
#include "Algorithm.hpp"
#include "EventFormatter.hpp"
#include "Position.hpp"
#include "Statistics.hpp"
#include "Logger.hpp"
#include "database/Database.hpp"

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
}

bool Listener::init(Algorithm& algorithm) {
    algorithm.onOpen.connect(std::bind(&Listener::handleOpen, this, std::placeholders::_1));
    algorithm.onAverage.connect(std::bind(&Listener::handleAverage, this, std::placeholders::_1));
    algorithm.onClose.connect(std::bind(&Listener::handleClose, this, std::placeholders::_1));
    algorithm.onStop.connect(std::bind(&Listener::handleStop, this, std::placeholders::_1));

    Logger::title(EventFormatter::title(_settings.symbol));

    _statistics = Statistics::create(_settings.uniqId() + ":stats");
    return true;
}

void Listener::update(const Position& position, const Context& context) {
    if (position.has() && not _settings.isBackTest()) {
        _status = EventFormatter::update(position, context);
        Logger::status(_status);
    } else _status = "";
}

void Listener::handleOpen(const Position& position) {
    std::string event = EventFormatter::order(position);
    Logger::info(event);

    sendEvent(event);
}

void Listener::handleAverage(const Position& position) {
    std::string event = EventFormatter::order(position);
    Logger::info(event);

    sendEvent(event);
}

void Listener::handleClose(const Report& report) {
    std::string event = EventFormatter::profit(report, _settings.symbol);
    Logger::info(event);

    // добавим в общий репорт
    _report.add(report);

    // сохраняем статистику закрытия сделки
    _statistics->report(report);
    if (_settings.isRelease())
        _statistics->save();

    sendEvent(event);
}

void Listener::handleStop(void*) {
    if (_report.positions == 0)
        return;

    std::string event = EventFormatter::report(_report, _settings.symbol);
    Logger::info(event);
}

const std::string& Listener::status() const {
    return _status;
}

void Listener::sendEvent(const std::string& event) const {
    if (not _settings.isRelease())
        return;

    DB().rpush(_settings.username + ":events", event);
}