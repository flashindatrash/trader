#pragma once

#include "Proxy.hpp"

namespace core {
    class Config;
}

class StockController;

class StockProxy : public core::Proxy<StockProxy> {
public: // methods
    StockProxy() = default;
    virtual ~StockProxy() override;

    void init(const core::Config& config);
    void run();

protected: // vars
    StockController* _controller = nullptr;
};

#define Stock() StockProxy::getInstance()
