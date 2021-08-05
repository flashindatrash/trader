#pragma once

#include <global.hpp>
#include "Proxy.hpp"

class BinanceSymbol;
class BinanceBookData;
class BinancePriceHistory;

class BinancePrices : public Core::Proxy<BinancePrices>
{
    typedef std::function<void(const BinanceBookData&)> Fn;

public: // methods
    BinancePrices() {}
    virtual ~BinancePrices() override;

    void init();

    void connect(const BinanceSymbol& symbol);
    int handle(Json::Value& json);

    void addListener(Fn listener);

    // last average price
    void setPrice(const BinanceSymbol& symbol, double price);
    double getPrice(const BinanceSymbol& symbol) const;

    // price history
    const BinancePriceHistory* getPriceHistory(const BinanceSymbol& symbol) const;

protected: // vars
    std::unordered_map<std::string, double> _prices;
    std::unordered_map<std::string, BinancePriceHistory*> _histories;
    std::vector<Fn> _listeners;
};

#define SBinancePrices() BinancePrices::getInstance()
