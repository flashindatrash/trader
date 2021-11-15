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

    std::string status() const;
    std::string statistics() const;

    void sendEvent(const std::string& event) const;

protected: // methods
    explicit Listener(Settings settings);

    void handleOpen(const Position& position);
    void handleAverage(const Position& position);
    void handleClose(const Report& report);
    void handleStop(void*);

protected: // vars
    const Settings _settings;

    Report _report;
    Formatter _status;

    Statistics* _statistics = nullptr;
};
NS_END

