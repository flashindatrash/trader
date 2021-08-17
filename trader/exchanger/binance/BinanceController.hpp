#pragma once

#include <thread>
#include <string>
#include <unordered_map>
#include "exchanger/base/ExchangerController.hpp"
#include "response/BinanceSymbolData.hpp"

namespace Json {
    class Value;
}

class BinanceController : public ExchangerController {
public: // methods
    BinanceController() = default;
    ~BinanceController() override;

public: // virtual    
    bool init(const core::Config& config) override;
    void run() override;
    void tick(time_t now) override;

    bool getSymbolInfo(Storage::Type_pair& container) const override;
    bool getAllPrices(Storage::Type_price& container) const override;
    bool getBalances(Storage::Type_balance& container) const override;
    bool getOrders(BookWrapper& wrapper) const override;
    bool getChart(ChartWrapper& wrapper, ChartInterval interval) const override;

    void connectPrices(Storage::Type_price& container) override;
    void connectBalances(Storage::Type_balance& container) override;

    const BookWrapper*          connectOrders(BookWrapper& wrapper) override;
    const CandlestickWrapper*   connectStats(CandlestickWrapper& wrapper) override;
    const ChartWrapper*         connectChart(ChartWrapper& wrapper, ChartInterval interval) override;

    const OrderWrapper* createOrder(const OrderRequest& request) override;

protected: // methods
    bool initUserListenKey();
    void startUserDataStream();
    void keepUserDataStream();
    void updateDailyChange();
    double getMinQuantity(const BinanceSymbolData& info) const;

protected: // callbacks
    int onUserDataStream(Json::Value& json);
    int onKlineDataStream(Json::Value& json);

private: // static vars
    static std::unordered_map<std::string, BinanceSymbolData> _symbols;

private: // vars
    std::thread _thread;

    Storage::Type_price* _prices_connector = nullptr;
    Storage::Type_balance* _balances_connector = nullptr;
    BookWrapper* _orders_connector = nullptr;
    ChartWrapper* _chart_connector = nullptr;
    CandlestickWrapper* _stats_connector = nullptr;

    std::string _stream_listen_key = "";
    time_t _time_start_userstream = 0;
    time_t _time_keep_userstream = 0;
    time_t _time_daily_change = 0;
};

