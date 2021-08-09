#pragma once

#include "Proxy.hpp"

class TradeSymbol;
class KlineHistory;
struct BinanceKlineData;

class BinanceKlines : public core::Proxy<BinanceKlines>, public core::Emitter<BinanceKlineData>
{
public: // methods
    BinanceKlines() {}
    virtual ~BinanceKlines() override;

    void init(const TradeSymbol& symbol);

    const KlineHistory* getHistory(const TradeSymbol& symbol) const;

protected: // methods
    int handle(Json::Value& json);

    void add(const BinanceKlineData& data);

protected: // vars
    std::unordered_map<std::string, KlineHistory*> _histories;
};

#define SKlines() BinanceKlines::getInstance()
