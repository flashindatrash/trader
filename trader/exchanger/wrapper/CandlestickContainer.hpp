#pragma once

#include <global.hpp>
#include "Proxy.hpp"
#include "exchanger/binance/response/BinanceKlineData.hpp"

class CandlestickContainer : public core::Emitter<BinanceKlineData> {
public: // static
    static CandlestickContainer* create();

public: // methods
    void add(const BinanceKlineData& data);

    const std::vector<BinanceKlineData>& klines() const;
    const BinanceKlineData& back() const;

protected: // methods
    CandlestickContainer() = default;

protected: // vars
    std::vector<BinanceKlineData> _klines;
};

