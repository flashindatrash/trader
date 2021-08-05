#pragma once

#include "Proxy.hpp"

class BinanceSymbol;
class BinancePriceHistory;
struct BinanceBookData;

class BinancePrices : public Core::Proxy<BinancePrices>, public Core::Emitter<BinanceBookData>
{
public: // methods
    BinancePrices() {}
    virtual ~BinancePrices() override;

    void init();
    void connect(const BinanceSymbol& symbol);

    // last average price
    void setPrice(const BinanceSymbol& symbol, double price);
    double getPrice(const BinanceSymbol& symbol) const;

    // price history
    const BinancePriceHistory* getPriceHistory(const BinanceSymbol& symbol) const;

protected: // methods
    int handle(Json::Value& json);

protected: // vars
    std::unordered_map<std::string, double> _prices;
    std::unordered_map<std::string, BinancePriceHistory*> _histories;
};

#define SPrices() BinancePrices::getInstance()
