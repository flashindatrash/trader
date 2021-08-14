#pragma once

#include <global.hpp>
#include "Proxy.hpp"
#include "exchanger/binance/response/BinanceKlineData.hpp"

class ChartWrapper : public core::Emitter<BinanceKlineData> {
public: // static
    static ChartWrapper* create();

public: // methods
    void add(const BinanceKlineData& data);

    const std::vector<BinanceKlineData>& klines() const;
    const BinanceKlineData& back() const;

protected: // methods
    ChartWrapper() = default;

protected: // vars
    std::vector<BinanceKlineData> _klines;
};

