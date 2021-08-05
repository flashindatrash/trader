#pragma once

#include "Proxy.hpp"

class BinanceSymbol;
class Algorithm;

class BinanceAlgorithm : public Core::Proxy<BinanceAlgorithm>
{
public: // methods
    BinanceAlgorithm() {}
    virtual ~BinanceAlgorithm() override;

    void init(const BinanceSymbol& symbol);

protected: // vars
    Algorithm* _algorithm = nullptr;
};

#define SAlgorithm() BinanceAlgorithm::getInstance()
