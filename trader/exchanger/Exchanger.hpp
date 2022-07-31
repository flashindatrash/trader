#pragma once

#include "core/Proxy.hpp"
#include "exchanger/base/Storage.hpp"
#include <cmath>

namespace core {
    class Config;
}

class ExchangerController;
class OrderWrapper;
struct ChartRequest;
struct OrderRequest;
struct StakingRequest;
enum ChartInterval : unsigned int;

class ExchangerProxy : public core::Proxy<ExchangerProxy>, public Storage {
public: // methods
    ExchangerProxy() = default;
    ~ExchangerProxy() override;

    bool init(const core::Config& config);
    void run();
    void stop();

    bool loadPairs();
    bool loadPrices();
    bool loadStakings();
    bool loadPrice(const std::string& key);
    bool loadOrders(const std::string& key);
    bool loadStats(const std::string& key);
    bool loadCharts(const std::string& key, ChartRequest& request);
    void listenCharts(const std::string& key, ChartInterval interval);
    void listenTickers(const std::string& key);
    void unlistenTickers(const std::string& key);
    bool updateStaking(const std::string& key);

    const OrderWrapper* createOrder(OrderRequest& request);
    bool stake(StakingRequest& request);

    double roundQuantity(double quantity, const std::string& key, double(*fn)(double) = std::round) const;
    double fee() const;

protected: // methods
    void tick(time_t now);

protected: // vars
    ExchangerController* _controller = nullptr;
};

#define Exchanger() ExchangerProxy::getInstance()
