#pragma once

#include "Proxy.hpp"
#include "Signal.hpp"
#include "exchanger/wrapper/SymbolInfo.hpp"
#include "exchanger/wrapper/SymbolSet.hpp"
#include "exchanger/wrapper/PriceContainer.hpp"
#include "exchanger/wrapper/Balance.hpp"

namespace core {
    class Config;
}

class ExchangerController;

class ExchangerProxy : public core::Proxy<ExchangerProxy> {
public: // methods
    ExchangerProxy() = default;
    virtual ~ExchangerProxy() override;

    void init(const core::Config& config);
    void run();

    CONTAINER(Symbol, SymbolInfo, info)
    CONTAINER(Symbol, PriceContainer, price)
    CONTAINER(Asset, Balance, balance)

public: // signals
    Signal<Asset> onBalanceChanged;

protected: // vars
    ExchangerController* _controller = nullptr;
};

#define Exchanger() ExchangerProxy::getInstance()
