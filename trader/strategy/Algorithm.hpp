#pragma once

#include "Signal.hpp"
#include "Settings.hpp"

struct OrderRequest;

NS_BEGIN
class Position;
class Script;
class Report;

class Algorithm {
public: // static
    static Algorithm* create(const Settings& settings);

public: // methods
    virtual ~Algorithm();

    bool init();
    void stop();

    bool tryOpen();
    bool tryClose();
    bool tryAverage();

    bool close();
    bool execute();

    const Position& position() const;

protected: // methods
    explicit Algorithm(const Settings& settings);

    bool createOrder(OrderRequest& request, Position& result) const;

public: // signals
    Signal<void*> onStop;
    Signal<Position&> onOpen;
    Signal<Position&> onAverage;
    Signal<Position&> onClose;
    Signal<Position&> onTick;
    Signal<Report&> onReport;

protected: // vars
    const Settings& _settings;

    Script* _script = nullptr;
    Position* _position = nullptr;
};
NS_END
