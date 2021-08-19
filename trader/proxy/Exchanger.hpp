#pragma once

#include "Proxy.hpp"
#include "exchanger/base/Storage.hpp"

namespace core {
    class Config;
}

class ExchangerController;
class OrderWrapper;
struct OrderRequest;
enum ChartInterval : unsigned int;

class ExchangerProxy : public core::Proxy<ExchangerProxy>, public Storage {
public: // methods
    ExchangerProxy() = default;
    virtual ~ExchangerProxy() override;

    bool init(const core::Config& config);
    void run();

    bool loadOrders(const std::string& key);
    void listenStats(const std::string& key);
    void listenCharts(const std::string& key, ChartInterval interval);

    const OrderWrapper* createOrder(const OrderRequest& request);

protected: // methods
    void tick(time_t now);

protected: // vars
    ExchangerController* _controller = nullptr;
};

#define Exchanger() ExchangerProxy::getInstance()
