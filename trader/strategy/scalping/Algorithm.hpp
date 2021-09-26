#pragma once

#include "Defines.hpp"
#include "Settings.hpp"

struct OrderRequest;

NS_BEGIN
class Context;
class Statistics;
class Position;
class Positions;

class Algorithm {
public: // static
    static Algorithm* create(const Settings& settings);

public: // methods
    virtual ~Algorithm();

    bool init();
    void execute(const Context& context);

protected: // methods
    explicit Algorithm(Settings settings);

    bool tryTakeProfit(const Context& context);
    bool tryStopLoss(const Context& context);
    bool tryOpen(const Context& context);
    bool tryClose(const Position& closable);

    bool createOrder(OrderRequest& request, Position& result) const;

    std::pair<OrderSide, double> risk() const;
    double risk(OrderSide side) const;

protected: // vars
    const Settings _settings;

    Positions* _positions = nullptr;
    Statistics* _statistics = nullptr;
};
NS_END
