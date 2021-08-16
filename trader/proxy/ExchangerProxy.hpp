#pragma once

#include "Proxy.hpp"
#include "exchanger/base/Storage-include.hpp"

namespace core {
    class Config;
}

class ExchangerController;
class OrderRequest;

class ExchangerProxy : public core::Proxy<ExchangerProxy>, public Storage {
public: // methods
    ExchangerProxy() = default;
    virtual ~ExchangerProxy() override;

    bool init(const core::Config& config, const Symbol& symbol);
    void run();

    const OrderWrapper* createOrder(const OrderRequest& request);

protected: // vars
    ExchangerController* _controller = nullptr;
};

#define Exchanger() ExchangerProxy::getInstance()
