#pragma once

#include "wrapper/Symbol.hpp"
#include "wrapper/SymbolSet.hpp"

namespace core {
    class Config;
}

class SymbolInfo;
class PriceContainer;
class Balance;

class ExchangerController {
public: // static
    static ExchangerController* create();

public: // virtual
    virtual ~ExchangerController() = default;
    virtual void init(const core::Config& config) = 0;
    virtual void run() = 0;

    virtual bool getSymbolInfo(SymbolSet<SymbolInfo>& result) = 0;
    virtual bool getAllPrices(SymbolSet<PriceContainer>& result) = 0;
    virtual bool getBalances(SymbolSet<Balance>& result) = 0;

protected: // methods
    ExchangerController() = default;
};

