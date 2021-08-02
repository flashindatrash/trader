#pragma once

#include "Proxy.hpp"

class BinanceExchangeInfo : public Core::Proxy<BinanceExchangeInfo>
{
public: // methods
    BinanceExchangeInfo();

    bool init() override;
};

#define SBinanceExchangeInfo() BinanceExchangeInfo::getInstance()
