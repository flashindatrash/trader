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
    ~ExchangerProxy() override;

    bool init(const core::Config& config);
    void run();
    void stop();

    bool loadOrders(const std::string& key);
    bool loadStats(const std::string& key);
    bool loadCharts(const std::string& key);
    void listenCharts(const std::string& key);
    void listenTickers(const std::string& key);

    const OrderWrapper* createOrder(OrderRequest& request);

    double minQuantity(const std::string& key) const;
    double fee() const;

protected: // methods
    void tick(time_t now);

protected: // vars
    ExchangerController* _controller = nullptr;
};

#define Exchanger() ExchangerProxy::getInstance()
