#pragma once

#include <string>
#include "Proxy.hpp"
#include "BinanceSymbol.hpp"

class BinanceBookData;

class BinanceAlgorithm : public Core::Proxy<BinanceAlgorithm>
{
public: // methods
    BinanceAlgorithm();

    bool init() override;
    void connect(const BinanceSymbol& symbol);

    void onBookData(const BinanceBookData& data);

protected: // vars
    BinanceSymbol _symbol;
    double _rate_up = 0.003;
    double _rate_down = 0.002;
    double _lot = 0.001;

    // last order
    double _last_price = 40292;
    std::string _last_side = "BUY";
};

#define SBinanceAlgorithm() BinanceAlgorithm::getInstance()
