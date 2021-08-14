#pragma once

#include "Proxy.hpp"

class Symbol;
class ChartWrapper;
struct BinanceKlineData;

class BinanceKlines : public core::Proxy<BinanceKlines>
{
public: // methods
    BinanceKlines() = default;
    virtual ~BinanceKlines() override;

    void init(const Symbol& symbol);

    ChartWrapper* get(const Symbol& symbol) const;

protected: // methods
    int handle(Json::Value& json);

    void add(const BinanceKlineData& data);

protected: // vars
    std::unordered_map<std::string, ChartWrapper*> _histories;
};

#define SKlines() BinanceKlines::getInstance()
