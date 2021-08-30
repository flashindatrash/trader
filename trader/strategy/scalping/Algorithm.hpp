#pragma once

#include "Defines.hpp"
#include "Settings.hpp"

NS_BEGIN
class Context;
class Positions;
class Statistics;

class Algorithm {
public: // static
    static Algorithm* create(const Settings& settings);

public: // methods
    virtual ~Algorithm();

    bool init();
    void execute(const Context& context);

protected: // methods
    explicit Algorithm(Settings settings);

    bool tryClosePosition(const Context& context);
    bool tryOpenPosition(const Context& context);

protected: // vars
    const Settings _settings;

    Positions* _positions = nullptr;
    Statistics* _statistics = nullptr;
};
NS_END
