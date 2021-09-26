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
public: // Range
    struct Range {
        const CandlestickWrapper* begin = nullptr;
        const CandlestickWrapper* end = nullptr;

        bool isValid() const;
    };

public: // static
    static ChartWrapper* create();
    static Signal<CandlestickWrapper> onCandleClosed;

public: // methods
    virtual ~ChartWrapper();

    const CandlestickWrapper* add(const Candlestick& data);
    const std::vector<CandlestickWrapper*>& get() const;
    const CandlestickWrapper* last() const;
    ChartWrapper::Range last(Price current, Price change) const;

    void setInterval(ChartInterval interval);
    const ChartInterval& interval() const;

protected: // methods
    ChartWrapper() = default;

protected: // vars
    std::vector<CandlestickWrapper*> _candlesticks;
    ChartInterval _interval = ChartInterval::m15;
};

