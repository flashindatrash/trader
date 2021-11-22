#pragma once

#include "Signal.hpp"
#include "Settings.hpp"

struct OrderRequest;

NS_BEGIN
class Position;
class Report;

class Algorithm {
public: // static
    static Algorithm* create(const Settings& settings);

public: // methods
    virtual ~Algorithm();

    bool init();
    void start();
    void stop();

    bool tryTakeProfit();
    bool tryStopLoss();
    bool tryAverage();
    bool tryOpen();
    bool tryClose();

    bool execute();

    const Position& position() const;

protected: // methods
    explicit Algorithm(Settings settings);

    bool createOrder(OrderRequest& request, Position& result) const;
    void indicator(OrderSide& trend, OrderSide& signal) const;

public: // signals
    Signal<void*> onStart;
    Signal<void*> onStop;
    Signal<Position&> onOpen;
    Signal<Position&> onAverage;
    Signal<Position&> onClose;
    Signal<Position&> onTick;
    Signal<Report&> onReport;

protected: // vars
    const Settings _settings;

    Position* _position = nullptr;
};
NS_END
