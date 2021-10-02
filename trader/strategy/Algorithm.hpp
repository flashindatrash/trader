#pragma once

#include "Defines.hpp"
#include "Settings.hpp"
#include "Report.hpp"

struct OrderRequest;

NS_BEGIN
class Context;
class Statistics;
class Position;

class Algorithm {
public: // static
    static Algorithm* create(const Settings& settings);

public: // methods
    virtual ~Algorithm();

    bool init();
    void execute(const Context& context);
    void report() const;

protected: // methods
    explicit Algorithm(Settings settings);

    bool tryTakeProfit(const Context& context);
    bool tryStopLoss(const Context& context);
    bool tryAverage(const Context& context);
    bool tryOpen(const Context& context);
    bool tryClose(const Context& context);

    bool createOrder(const Context& context, OrderRequest& request, Position& result) const;
    OrderSide getSignal(const Context& context) const;

protected: // vars
    const Settings _settings;
    Report _report;

    Position* _position = nullptr;
    Statistics* _statistics = nullptr;
};
NS_END
