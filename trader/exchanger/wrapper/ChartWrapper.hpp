#pragma once

#include "Signal.hpp"
#include "exchanger/base/Identifier.hpp"

struct Candlestick;
class CandlestickWrapper;

enum ChartInterval : unsigned int {
    m5,
    m15
};

class ChartWrapper : public Identifier {
public: // static
    static ChartWrapper* create();
    static Signal<const CandlestickWrapper&> onCandleClosed;

public: // methods
    virtual ~ChartWrapper();

    const CandlestickWrapper* add(const Candlestick& data);
    const std::vector<CandlestickWrapper*>& get() const;
    const CandlestickWrapper* last() const;

protected: // methods
    ChartWrapper() = default;

protected: // vars
    std::vector<CandlestickWrapper*> _candlesticks;
};

