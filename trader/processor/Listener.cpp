//
// Created by Вадим Проскурин on 06.11.2021.
//

#include "processor/Listener.hpp"
#include "Context.hpp"
#include "Formatter.hpp"
#include "Algorithm.hpp"
#include "core/Logger.hpp"
#include "base/Settings.hpp"
#include "exchanger/base/Position.hpp"

using namespace trader;

Listener* Listener::create(const Settings& settings) {
    auto* listener = new Listener(settings);
    return listener;
}

Listener::Listener(const Settings& settings)
    : _settings(settings)
    , _stats(settings.username(), settings.symbol())
{
}

bool Listener::init(Algorithm& algorithm) {
    algorithm.onStop.connect(std::bind(&Listener::handleStop, this, std::placeholders::_1));
    algorithm.onOpen.connect(std::bind(&Listener::handlePosition, this, std::placeholders::_1));
    algorithm.onAverage.connect(std::bind(&Listener::handlePosition, this, std::placeholders::_1));
    algorithm.onClose.connect(std::bind(&Listener::handlePosition, this, std::placeholders::_1));
    algorithm.onTick.connect(std::bind(&Listener::handleTick, this, std::placeholders::_1));
    algorithm.onReport.connect(std::bind(&Listener::handleReport, this, std::placeholders::_1));

    Logger::title(Formatter::title(_settings.symbol()).terminal());
    return true;
}

void Listener::handleStop(void*) {
    if (_report.positions == 0)
        return;

    Formatter event = Formatter::report(_report, _settings.symbol());
    Logger::info(event.terminal());

    _report = Report();
}

void Listener::handlePosition(const Position& position) {
    Formatter event = Formatter::order(position);

    if (_settings.logEnabled())
        Logger::info(event.terminal());

}

void Listener::handleTick(const Position& position) {
    if (Context::current == nullptr || not position.has())
        return;

    if (_settings.isBackTest())
        return;

    Formatter event = Formatter::update(position, *Context::current, _settings);
    Logger::status(event.terminal());
}

void Listener::handleReport(const Report& report) {
    // добавим в общий репорт
    _report.add(report);

    // формируем эвент
    Formatter event = Formatter::profit(report, _settings.symbol());

    if (_settings.logEnabled())
        Logger::info(event.terminal());

    if (_settings.isRelease()) {
        // сохраняем статистику пары
        _stats.setProfit(report.profit);
        _stats.setEarnBase(report.earn_base);
        _stats.setEarnQuote(report.earn_quote);
        _stats.save();

    }
}
