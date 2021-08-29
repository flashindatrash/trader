#pragma once

#include "Defines.hpp"
#include "Settings.hpp"

NS_BEGIN
class Context;
class Positions;
class Statistics;
class Status;

class Algorithm {
public: // static
    static Algorithm* create(const Settings& settings);

    enum Result {
        OK,
        INVALID,
        FAILED,
        NOT_ENOUGH,
        CLOSE_NOT_EXISTS,
        CLOSE_NON_PROFITABLE,
        OPEN_WAIT_PROFIT,
        OPEN_LIMIT,
        OPEN_PROFIT_SUPPLY
    };

public: // methods
    virtual ~Algorithm();

    bool init();
    void execute(const Context& context);

protected: // methods
    explicit Algorithm(Settings settings);

    Result tryClosePosition(const Context& context);
    Result tryOpenPosition(const Context& context);

protected: // vars
    const Settings _settings;

    Positions* _positions = nullptr;
    Statistics* _statistics = nullptr;
    Status* _status = nullptr;
};
NS_END
