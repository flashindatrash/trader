//
// Created by Вадим Проскурин on 06.11.2021.
//

#pragma once

#include "Settings.hpp"
#include "Report.hpp"
#include "Pair.hpp"

NS_BEGIN
class Algorithm;
class Position;

class Listener {
public: // static
    static Listener* create(const Settings& settings);

public: // methods
    bool init(Algorithm& algorithm);

protected: // methods
    explicit Listener(Settings settings);

    void handleStart(void*);
    void handleStop(void*);
    void handlePosition(const Position& position);
    void handleTick(const Position& position);
    void handleReport(const Report& report);

protected: // vars
    Report _report;
    const Settings _settings;
    protocol::Pair _pair;
};
NS_END

