//
// Created by Вадим Проскурин on 06.11.2021.
//

#pragma once

#include "Settings.hpp"
#include "Report.hpp"
#include "Trader.hpp"

NS_BEGIN
class Algorithm;
class Position;

class Listener {
public: // static
    static Listener* create(const Settings& settings);

public: // methods
    bool init(Algorithm& algorithm);

protected: // methods
    explicit Listener(const Settings& settings);

    void handleStart(void*);
    void handleStop(void*);
    void handlePosition(const Position& position);
    void handleTick(const Position& position);
    void handleReport(const Report& report);

protected: // vars
    const Settings& _settings;
    Report _report;
    protocol::Trader _stats;
};
NS_END

