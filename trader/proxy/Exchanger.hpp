#pragma once

#include "Proxy.hpp"
#include "exchanger/base/Storage.hpp"

namespace core {
    class Config;
}

class Symbol;
class ExchangerController;
class OrderWrapper;
struct OrderRequest;

class ExchangerProxy : public core::Proxy<ExchangerProxy>, public Storage {
public: // methods
    ExchangerProxy() = default;
    virtual ~ExchangerProxy() override;

    bool init(const core::Config& config);
    void run();

    const OrderWrapper* createOrder(const OrderRequest& request);

protected: // vars
    ExchangerController* _controller = nullptr;
};

#define Exchanger() ExchangerProxy::getInstance()
