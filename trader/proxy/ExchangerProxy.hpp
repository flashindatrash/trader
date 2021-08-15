#pragma once

#include "Proxy.hpp"
#include "exchanger/base/Storage-include.hpp"

namespace core {
    class Config;
}

class ExchangerController;
class CandlestickWrapper;

class ExchangerProxy : public core::Proxy<ExchangerProxy>, public Storage {
public: // methods
    ExchangerProxy() = default;
    virtual ~ExchangerProxy() override;

    bool init(const core::Config& config);
    void connect(const Symbol& symbol);
    void run();

protected: // vars
    ExchangerController* _controller = nullptr;
};

#define Exchanger() ExchangerProxy::getInstance()
