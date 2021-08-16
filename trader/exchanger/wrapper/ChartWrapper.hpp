#pragma once

#include <string>
#include "exchanger/base/Storage.hpp"

struct Candlestick;
class CandlestickWrapper;

enum ChartInterval {
    m5,
    m15
};

class ChartWrapper : public MapIdentifier<std::string> {
public: // static
    static ChartWrapper* create();
    static Signal<const CandlestickWrapper&> onCandleClosed;

public: // methods
    virtual ~ChartWrapper();

    const CandlestickWrapper* add(const Candlestick& data);
    const std::vector<CandlestickWrapper*>& get() const;

protected: // methods
    ChartWrapper() = default;

protected: // vars
    std::vector<CandlestickWrapper*> _candlesticks;
};

