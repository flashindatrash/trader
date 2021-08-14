#pragma once

#include <thread>
#include <string>
#include "exchanger/base/ExchangerController.hpp"

namespace Json {
    class Value;
}

class BinanceController : public ExchangerController {
public: // methods
    BinanceController() = default;
    ~BinanceController() override;

public: // virtual    
    void init(const core::Config& config) override;
    void run() override;
    void tick(time_t now) override;

    bool getSymbolInfo(Storage::Type_info& container) const override;
    bool getAllPrices(Storage::Type_price& container) const override;
    bool getBalances(Storage::Type_balance& container) const override;

    void connectBalances(Storage::Type_balance& container) override;
    void connectDailyChange(KlineWrapper& wrapper) override;

protected: // methods
    void initUserListenKey();
    void startUserDataStream();
    void keepUserDataStream();
    void updateDailyChange();

protected: // callbacks
    int onUserDataStream(Json::Value& json);

protected: // vars
    // websoket thread
    std::thread _thread;
    // user data stream
    std::string _stream_listen_key = "";
    // balance connector
    Storage::Type_balance* _connect_balances = nullptr;
    time_t _time_userstream = 0;
    // daily change connector
    KlineWrapper* _connect_daily_change = nullptr;
    time_t _time_daily_change = 0;
};

