#pragma once

#include "Proxy.hpp"

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

protected: // vars
    ExchangerController* _controller = nullptr;
};

#define Exchanger() ExchangerProxy::getInstance()
