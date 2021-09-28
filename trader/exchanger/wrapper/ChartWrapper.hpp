#pragma once

#include "Signal.hpp"
#include "exchanger/base/Identifier.hpp"
#include "exchanger/base/OrderBase.hpp"

struct Candlestick;
class CandlestickWrapper;

enum ChartInterval : unsigned int {
    m5,
    m15
};

class ChartWrapper : public Identifier {
public: // static
    typedef std::vector<CandlestickWrapper*>::const_iterator ConstIterator;

    static ChartWrapper* create();
    static Signal<CandlestickWrapper> onCandleClosed;

public: // methods
    virtual ~ChartWrapper();

    const CandlestickWrapper* add(const Candlestick& data);
    const std::vector<CandlestickWrapper*>& get() const;

    void setInterval(ChartInterval interval);
    const ChartInterval& interval() const;

    Price ema(ConstIterator end, size_t length) const;

protected: // methods
    ChartWrapper() = default;

protected: // vars
    std::vector<CandlestickWrapper*> _candlesticks;
    ChartInterval _interval = ChartInterval::m15;
};

