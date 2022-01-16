//
// Created by Вадим Проскурин on 06.11.2021.
//

#pragma once

#include "Report.hpp"
#include "protocol/Stats.hpp"

class Position;
struct Settings;

namespace pair {
class Algorithm;

class Listener {
public: // static
    static Listener* create(const Settings& settings);

public: // methods
    bool init(Algorithm& algorithm);

protected: // methods
    explicit Listener(const Settings& settings);

    void handleStop(void*);
    void handlePosition(const Position& position);
    void handleTick(const Position& position);
    void handleReport(const Report& report);

protected: // vars
    const Settings& _settings;
    Report _report;
    protocol::Stats _stats;
};
}

