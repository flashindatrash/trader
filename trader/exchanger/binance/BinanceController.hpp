#pragma once

#include <thread>
#include <string>
#include <unordered_map>
#include <vector>
#include "exchanger/abstract/ExchangerController.hpp"
#include "response/BinanceSymbolData.hpp"

namespace Json {
    class Value;
}

class BinanceWebsocket;

class BinanceController : public ExchangerController {
public: // methods
    BinanceController() = default;
    ~BinanceController() override;

public: // virtual
    bool init(const core::Config& config) override;
    void run() override;
    void tick(time_t now) override;

    void connectPrices(Storage::Type_price& container) override;
    void connectBalances(Storage::Type_balance& container) override;
    void connectCharts(Storage::Type_chart& container) override;

    bool loadPairs(Storage::Type_pair& container) const override;
    bool loadPrices(Storage::Type_price& container) const override;
    bool loadBalances(Storage::Type_balance& container) const override;
    bool loadOrders(BookWrapper& container) const override;
    bool loadStats(CandlestickWrapper& container) const override;
    bool loadCharts(ChartWrapper& container, ChartRequest& request) const override;

    void listenCharts(ChartWrapper& container, ChartInterval interval) override;
    void listenTicker(PriceWrapper& container) override;

    const OrderWrapper* createOrder(BookWrapper& container, OrderRequest& request) override;

    double roundQuantity(double quantity, const std::string& symbol, double(*fn)(double)) const override;
    double fee() const override;

protected: // methods
    bool initUserListenKey();
    void keepUserDataStream();

    bool loadSavings(Storage::Type_balance& container) const;

    double minQuantity(const std::string& symbol) const;

protected: // callbacks
    void onUserDataStream(const Json::Value& json);
    void onKlineDataStream(const Json::Value& json);
    void onTickerDataStream(const Json::Value& json);

private: // static vars
    static std::unordered_map<std::string, BinanceSymbolData> _symbols;

private: // vars
    std::thread _thread;
    std::vector<BinanceWebsocket*> _websockets;

    // number of milliseconds after timestamp the request is valid
    unsigned int _config_recv_window = 5000;
    // order fee percentage
    double _config_fee = 0.0;

    Storage::Type_price* _prices_connector = nullptr;
    Storage::Type_balance* _balances_connector = nullptr;
    Storage::Type_chart* _charts_connector = nullptr;

    time_t _time_keep_userstream = 0;
};

