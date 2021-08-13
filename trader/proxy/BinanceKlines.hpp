#pragma once

#include "Proxy.hpp"

class Symbol;
class CandlestickContainer;
struct BinanceKlineData;

class BinanceKlines : public core::Proxy<BinanceKlines>, public core::Emitter<BinanceKlineData>
{
public: // methods
    BinanceKlines() = default;
    virtual ~BinanceKlines() override;

    void init(const Symbol& symbol);

    CandlestickContainer* get(const Symbol& symbol) const;

protected: // methods
    int handle(Json::Value& json);

    void add(const BinanceKlineData& data);

protected: // vars
    std::unordered_map<std::string, CandlestickContainer*> _histories;
};

#define SKlines() BinanceKlines::getInstance()
