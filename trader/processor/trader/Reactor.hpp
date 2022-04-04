//
// Created by Вадим Проскурин on 06.11.2021.
//

#pragma once

class Settings;

namespace trader {
class Algorithm;

class Reactor {
public: // static
    static Reactor* create(Algorithm& algorithm, const Settings& settings);

public: // methods
    bool init();

    void execute() const;

protected: // methods
    explicit Reactor(Algorithm& algorithm, const Settings& settings);

protected: // vars
    Algorithm& _algorithm;
    const Settings& _settings;

};
}