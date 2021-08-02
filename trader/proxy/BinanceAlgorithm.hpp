#pragma once

#include "Proxy.hpp"
#include "BinanceSymbol.hpp"
#include "data/BinanceOrderData.hpp"

class BinanceBookData;

class BinanceAlgorithm : public Core::Proxy<BinanceAlgorithm>
{
public: // methods
    BinanceAlgorithm() {}

    void init(const BinanceSymbol& symbol);

protected: // methods
    void onBookData(const BinanceBookData& data);

protected: // vars
    BinanceSymbol _symbol;
    double _rate_up = 0.003;
    double _rate_down = 0.002;
    double _lot = 0.001;

    std::vector<BinanceOrderData> _orderHistory;
    // last order
    double _last_price = 0.0;
    std::string _last_side = "";
};

#define SBinanceAlgorithm() BinanceAlgorithm::getInstance()
