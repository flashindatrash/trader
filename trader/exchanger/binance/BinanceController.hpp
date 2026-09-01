#pragma once

#include <thread>
#include "exchanger/abstract/ExchangerController.hpp"
#include "response/BinanceExchangeData.hpp"

namespace Json {
    class Value;
}

class BinanceWebsocket;
class WalletRequest;

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
    bool loadCharts(ChartWrapper& container, ChartRequest& request) const override;

    void listenCharts(ChartWrapper& container, ChartInterval interval) override;
    void listenTicker(PriceWrapper& container) override;

    const OrderWrapper* createOrder(BookWrapper& container, OrderRequest& request) override;

    Decimal roundQuantity(Decimal quantity, const std::string& symbol) const override;
    double fee() const override;

protected: // methods
    bool loadPrices(Storage::Type_price& container) const;
    bool loadBalances(Storage::Type_balance& container) const;

    bool initUserListenKey();
    bool keepUserDataStream();

    bool addWebsocket(BinanceWebsocket* websocket);
    std::vector<BinanceWebsocket*>::iterator findWebsocket(const std::string& path);

    int checkError(const Json::Value& json, const std::string& context) const;
    bool checkServerTime() const;
    bool checkRateLimits() const;
    bool checkWalletRequest(WalletRequest& request, const std::string& asset, const Decimal& quantity) const;

    Decimal minQuantity(const std::string& symbol) const;

    void print(const std::string& context, const std::string& msg) const;

protected: // savings
    bool redeemSavings(const std::string& asset, Decimal quantity) const;

protected: // callbacks
    void onUserDataStream(const Json::Value& json);
    void onKlineDataStream(const Json::Value& json);
    void onTickerDataStream(const Json::Value& json);

private: // static vars
    static BinanceExchangeData _exchange_info;
    static double _commission;

private: // vars
    std::thread _thread;
    std::vector<BinanceWebsocket*> _websockets;

    // number of milliseconds after timestamp the request is valid
    unsigned int _config_recv_window = 5000;

    Storage::Type_price* _prices_connector = nullptr;
    Storage::Type_balance* _balances_connector = nullptr;
    Storage::Type_chart* _charts_connector = nullptr;

    time_t _time_keep_userstream = 0;
};
