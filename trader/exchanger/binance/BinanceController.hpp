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

protected: // methods
    void initUserListenKey();
    void startUserDataStream();

protected: // callbacks
    int onUserDataStream(Json::Value& json);

protected: // vars
    // websoket thread
    std::thread _thread;
    // user data stream
    std::string _stream_listen_key = "";
    time_t _stream_keep_alive = 0;
    // connectors
    Storage::Type_balance* _connect_balances = nullptr;
};

