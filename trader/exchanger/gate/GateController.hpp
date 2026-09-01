#pragma once

#include "exchanger/abstract/ExchangerController.hpp"
#include <atomic>
#include <functional>
#include <thread>
#include <unordered_map>
#include <vector>

namespace Json { class Value; }
class GateWebsocket;

class GateController : public ExchangerController {
public:
    GateController() = default;
    ~GateController() override;

    bool init(const core::Config& config) override;
    void run() override;
    void tick(time_t now) override;
    void connectPrices(Storage::Type_price& container) override;
    void connectBalances(Storage::Type_balance& container) override;
    void connectCharts(Storage::Type_chart& container) override;
    bool loadPairs(Storage::Type_pair& container) const override;
    bool loadCharts(ChartWrapper& container, ChartRequest& request) const override;
    void listenCharts(ChartWrapper& container, ChartInterval interval) override;
    void listenTicker(PriceWrapper& container) override;
    const OrderWrapper* createOrder(BookWrapper& container, OrderRequest& request) override;
    Decimal roundQuantity(Decimal quantity, const std::string& symbol) const override;
    double fee() const override;

private:
    struct PairInfo {
        std::string id, base, quote;
        Decimal min_base;
        unsigned amount_precision = 8;
        bool tradable = false;
    };

    bool request(const std::string& method, const std::string& path, const std::string& query,
                 const std::string& body, bool authenticated, Json::Value& result) const;
    bool loadPrices(Storage::Type_price& container) const;
    bool loadBalances(Storage::Type_balance& container) const;
    std::string pairId(const std::string& symbol) const;
    static std::string symbolId(const std::string& pair);
    static std::string interval(ChartInterval value);
    void addWebsocket(const std::string& channel, const std::string& payload,
                      std::function<void(const Json::Value&)> callback);
    void onTicker(const Json::Value& json);
    void onCandle(const Json::Value& json);
    void print(const std::string& context, const std::string& message) const;

    std::string _api_key, _secret_key, _api_url = "https://api.gateio.ws/api/v4";
    mutable std::unordered_map<std::string, PairInfo> _pairs;
    mutable double _commission = 0.002;
    Storage::Type_price* _prices = nullptr;
    Storage::Type_balance* _balances = nullptr;
    Storage::Type_chart* _charts = nullptr;
    std::vector<GateWebsocket*> _websockets;
    std::thread _thread;
    std::atomic_bool _running{false};
};
