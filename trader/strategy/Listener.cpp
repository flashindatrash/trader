//
// Created by Вадим Проскурин on 06.11.2021.
//

#include "Listener.hpp"

#include <utility>
#include "Algorithm.hpp"
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
    delete _statistics;
    _statistics = nullptr;
}

bool Listener::init(Algorithm& algorithm) {
    algorithm.onOpen.connect(std::bind(&Listener::handleOpen, this, std::placeholders::_1));
    algorithm.onAverage.connect(std::bind(&Listener::handleAverage, this, std::placeholders::_1));
    algorithm.onClose.connect(std::bind(&Listener::handleClose, this, std::placeholders::_1));
    algorithm.onStop.connect(std::bind(&Listener::handleStop, this, std::placeholders::_1));

    Logger::title(Formatter::title(_settings.symbol).terminal());

    _statistics = Statistics::create(_settings.storage("stats"));
    return true;
}

void Listener::update(const Position& position, const Context& context) {
    if (position.has() && not _settings.isBackTest()) {
        _status = Formatter::update(position, context);
        Logger::status(_status.terminal());
    } else _status = Formatter();
}

void Listener::handleOpen(const Position& position) {
    Formatter event = Formatter::order(position);
    Logger::info(event.terminal());

    sendEvent(event.html());
}

void Listener::handleAverage(const Position& position) {
    Formatter event = Formatter::order(position);
    Logger::info(event.terminal());

    sendEvent(event.html());
}

void Listener::handleClose(const Report& report) {
    Formatter event = Formatter::profit(report, _settings.symbol);
    Logger::info(event.terminal());

    // добавим в общий репорт
    _report.add(report);

    // сохраняем статистику закрытия сделки
    _statistics->report(report);
    if (_settings.isRelease())
        _statistics->save();

    sendEvent(event.html());
}

void Listener::handleStop(void*) {
    if (_report.positions == 0)
        return;

    Formatter event = Formatter::report(_report, _settings.symbol);
    Logger::info(event.terminal());
}

std::string Listener::status() const {
    return _status.html();
}

std::string Listener::statistics() const {
    return Formatter::stats(*_statistics, _settings.symbol).html();
}

void Listener::sendEvent(const std::string& event) const {
    if (not _settings.isRelease() || event.empty())
        return;

    DB().rpush(_settings.username + ":events", event);
}