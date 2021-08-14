#pragma once

#include <thread>
#include <string>
#include "exchanger/ExchangerController.hpp"

class BinanceController : public ExchangerController {
public: // methods
    BinanceController() = default;
    ~BinanceController() override;

public: // virtual    
    void init(const core::Config& config) override;
    void run() override;
    void tick(time_t now) override;

    bool getSymbolInfo(SymbolSet<SymbolInfo>& result) override;
    bool getAllPrices(SymbolSet<PriceContainer>& result) override;
    bool getBalances(SymbolSet<Balance>& result) override;

    void connectBalances(SymbolSet<Balance>& result, Signal<Asset>& signal) override;

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
    // balance container
    SymbolSet<Balance>* _balance_container = nullptr;
    Signal<Asset>* _balance_signal = nullptr;
};

