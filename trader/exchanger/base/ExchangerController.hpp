#pragma once

#include "exchanger/base/Storage.hpp"

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
    virtual void tick(time_t now) = 0;

    virtual bool getSymbolInfo(Storage::Type_info& container) const = 0;
    virtual bool getAllPrices(Storage::Type_price& container) const = 0;
    virtual bool getBalances(Storage::Type_balance& container) const = 0;

    virtual void connectBalances(Storage::Type_balance& container) = 0;

protected: // methods
    ExchangerController() = default;
};

