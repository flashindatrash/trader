#pragma once

#include <memory>
#include "core/Proxy.hpp"
#include "exchanger/base/Storage.hpp"
#include "exchanger/base/Decimal.hpp"

namespace core {
    class Config;
}

class ExchangerController;
class OrderWrapper;
struct ChartRequest;
struct OrderRequest;
enum ChartInterval : unsigned int;

class ExchangerProxy : public core::Proxy<ExchangerProxy>, public Storage {
public: // methods
    ExchangerProxy();
    ~ExchangerProxy() override;

    bool init(const core::Config& config);
    void run();
    void stop();

    bool loadCharts(const std::string& key, ChartRequest& request);
    void listenCharts(const std::string& key, ChartInterval interval);
    void listenTickers(const std::string& key);
    const OrderWrapper* createOrder(OrderRequest& request);
    Decimal roundQuantity(Decimal quantity, const std::string& key) const;
    double fee() const;

protected: // methods
    bool loadPairs();
    void tick(time_t now);

protected: // vars
    std::unique_ptr<ExchangerController> _controller;
};

#define Exchanger() ExchangerProxy::getInstance()
