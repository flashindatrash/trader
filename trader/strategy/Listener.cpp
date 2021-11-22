//
// Created by Вадим Проскурин on 06.11.2021.
//

#include "Listener.hpp"

#include <utility>
#include "Algorithm.hpp"
#include "Position.hpp"
#include "Statistics.hpp"
#include "Logger.hpp"
#include "Event.hpp"
#include "Context.hpp"

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
    algorithm.onStart.connect(std::bind(&Listener::handleStart, this, std::placeholders::_1));
    algorithm.onStop.connect(std::bind(&Listener::handleStop, this, std::placeholders::_1));
    algorithm.onOpen.connect(std::bind(&Listener::handlePosition, this, std::placeholders::_1));
    algorithm.onAverage.connect(std::bind(&Listener::handlePosition, this, std::placeholders::_1));
    algorithm.onClose.connect(std::bind(&Listener::handlePosition, this, std::placeholders::_1));
    algorithm.onTick.connect(std::bind(&Listener::handleTick, this, std::placeholders::_1));
    algorithm.onReport.connect(std::bind(&Listener::handleReport, this, std::placeholders::_1));

    Logger::title(Formatter::title(_settings.symbol).terminal());

    _statistics = Statistics::create(_settings.storage("stats"));
    return true;
}

void Listener::handleStart(void*) {
    Formatter event = Formatter::settings(_settings);
    Logger::info(event.terminal());
}

void Listener::handleStop(void*) {
    if (_report.positions == 0)
        return;

    Formatter event = Formatter::report(_report, _settings.symbol);
    Logger::info(event.terminal());
}

void Listener::handlePosition(const Position& position) {
    Formatter event = Formatter::order(position);
    Logger::info(event.terminal());

    if (_settings.isRelease()) {
        protocol::Event::add(_settings.username, event.html());
    }
}

void Listener::handleTick(const Position& position) {
    if (Context::current == nullptr || not position.has())
        return;

    Formatter event = Formatter::update(position, *Context::current);
    Logger::status(event.terminal());
}

void Listener::handleReport(const Report& report) {
    Formatter event = Formatter::profit(report, _settings.symbol);
    Logger::info(event.terminal());

    // добавим в общий репорт
    _report.add(report);

    // сохраняем статистику закрытия сделки
    _statistics->report(report);

    if (_settings.isRelease()) {
        _statistics->save();
        protocol::Event::add(_settings.username, event.html());
    }
}

/*Formatter Listener::statistics() const {
    return Formatter::stats(*_statistics, _settings.symbol);
}*/