#pragma once

#include "Proxy.hpp"
#include "exchanger/wrapper/SymbolInfo.hpp"
#include "exchanger/wrapper/SymbolSet.hpp"
#include "exchanger/wrapper/PriceContainer.hpp"

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

    CONTAINER(SymbolInfo, info)
    CONTAINER(PriceContainer, price)

protected: // vars
    ExchangerController* _controller = nullptr;
};

#define Exchanger() ExchangerProxy::getInstance()
