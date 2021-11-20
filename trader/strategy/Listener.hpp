//
// Created by Вадим Проскурин on 06.11.2021.
//

#pragma once

#include "Defines.hpp"
#include "Settings.hpp"
#include "Report.hpp"
#include "Formatter.hpp"

NS_BEGIN
class Algorithm;
class Statistics;
class Position;
class Context;

class Listener {
public: // static
    static Listener* create(const Settings& settings);

public: // methods
    virtual ~Listener();

    bool init(Algorithm& algorithm);
    void update(const Position& position, const Context& context);

    Formatter status() const;
    Formatter statistics() const;

    void sendEvent(const Formatter& event) const;

protected: // methods
    explicit Listener(Settings settings);

    void handleStart(void*);
    void handleStop(void*);
    void handlePosition(const Position& position);
    void handleReport(const Report& report);

protected: // vars
    const Settings _settings;

    Report _report;
    Formatter _status;

    Statistics* _statistics = nullptr;
};
NS_END

