#pragma once

#include "Defines.hpp"

NS_BEGIN
class Settings;
class Context;
class Positions;

class Algorithm {
public: // static
    static Algorithm* create(const Settings& settings);

public: // methods
    bool init();
    void execute(const Context& context);

protected: // methods
    Algorithm(const Settings& settings);

    bool tryClosePosition(const Context& context);
    bool tryOpenPosition(const Context& context);

protected: // vars
    const Settings& _settings;

    Positions* _positions = nullptr;
};
NS_END
