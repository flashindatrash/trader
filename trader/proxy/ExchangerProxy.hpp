#pragma once

#include "Proxy.hpp"
#include "Signal.hpp"
#include "exchanger/base/Storage.hpp"

namespace core {
    class Config;
}

class ExchangerController;

class ExchangerProxy : public core::Proxy<ExchangerProxy>, public Storage {
public: // methods
    ExchangerProxy() = default;
    virtual ~ExchangerProxy() override;

    void init(const core::Config& config);
    void run();

protected: // vars
    ExchangerController* _controller = nullptr;
};

#define Exchanger() ExchangerProxy::getInstance()
