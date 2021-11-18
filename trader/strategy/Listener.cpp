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
    algorithm.onOpen.connect(std::bind(&Listener::handlePosition, this, std::placeholders::_1));
    algorithm.onAverage.connect(std::bind(&Listener::handlePosition, this, std::placeholders::_1));
    algorithm.onClose.connect(std::bind(&Listener::handlePosition, this, std::placeholders::_1));
    algorithm.onReport.connect(std::bind(&Listener::handleReport, this, std::placeholders::_1));
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

void Listener::handlePosition(const Position& position) {
    Formatter event = Formatter::order(position);
    Logger::info(event.terminal());

    sendEvent(event);
}

void Listener::handleReport(const Report& report) {
    Formatter event = Formatter::profit(report, _settings.symbol);
    Logger::info(event.terminal());

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

    Formatter event = Formatter::report(_report, _settings.symbol);
    Logger::info(event.terminal());
}

Formatter Listener::status() const {
    return _status;
}

Formatter Listener::statistics() const {
    return Formatter::stats(*_statistics, _settings.symbol);
}

void Listener::sendEvent(const Formatter& event) const {
    if (not _settings.isRelease())
        return;

    std::string text = event.html();
    if (text.empty())
        return;

    DB().rpush(_settings.username + ":events", text);
}