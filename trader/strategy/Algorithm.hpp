#pragma once

#include "Signal.hpp"
#include "Defines.hpp"
#include "Settings.hpp"

struct OrderRequest;

NS_BEGIN
class Context;
class Position;
class Report;

class Algorithm {
public: // static
    static Algorithm* create(const Settings& settings);

public: // methods
    virtual ~Algorithm();

    bool init();
    void stop();

    const Position& execute(const Context& context);

protected: // methods
    explicit Algorithm(Settings settings);

    bool tryTakeProfit(const Context& context);
    bool tryStopLoss(const Context& context);
    bool tryAverage(const Context& context);
    bool tryOpen(const Context& context);
    bool tryClose(const Context& context);

    bool createOrder(const Context& context, OrderRequest& request, Position& result) const;
    void indicator(const Context& context, OrderSide& trend, OrderSide& signal) const;
    int availableAverage() const;

public: // signals
    Signal<Position&> onOpen;
    Signal<Position&> onAverage;
    Signal<Report&> onClose;
    Signal<void*> onStop;

protected: // vars
    const Settings _settings;

    Position* _position = nullptr;
};
NS_END
