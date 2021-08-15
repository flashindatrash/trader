#pragma once

#include "Proxy.hpp"
#include "exchanger/base/Storage.hpp"

struct Candlestick;
class CandlestickWrapper;

class ChartWrapper : public MapIdentifier<std::string> {
public: // static
    static ChartWrapper* create();

public: // methods
    bool add(const Candlestick& data);

    const std::vector<CandlestickWrapper*>& klines() const;

public: // signals
    Signal<const CandlestickWrapper&> onCandleClosed;

protected: // methods
    ChartWrapper() = default;

protected: // vars
    std::vector<CandlestickWrapper*> _candlesticks;
};

