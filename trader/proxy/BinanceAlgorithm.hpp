#pragma once

#include "Proxy.hpp"
#include "wrapper/BinanceSymbol.hpp"

class BinanceBookData;
class BaseAlgorithm;

class BinanceAlgorithm : public Core::Proxy<BinanceAlgorithm>
{
public: // methods
    BinanceAlgorithm() {}
    virtual ~BinanceAlgorithm() override;

    void init(const BinanceSymbol& symbol);

protected: // methods
    void handle(const BinanceBookData& data);

protected: // vars
    BaseAlgorithm* _algorithm = nullptr;
};

#define SBinanceAlgorithm() BinanceAlgorithm::getInstance()
