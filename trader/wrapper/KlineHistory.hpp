#pragma once

#include "data/BinanceKlineData.hpp"

class KlineHistory
{
public: // static
    static KlineHistory* create();

public: // methods
    void add(const BinanceKlineData& data);

    const std::vector<BinanceKlineData>& klines() const;
    const BinanceKlineData& back() const;

protected: // methods
    KlineHistory() = default;

protected: // vars
    std::vector<BinanceKlineData> _klines;
};

