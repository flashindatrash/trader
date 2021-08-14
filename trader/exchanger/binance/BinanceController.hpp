#pragma once

#include <thread>
#include "exchanger/ExchangerController.hpp"

class BinanceController : public ExchangerController {
public: // methods
    BinanceController() = default;

public: // virtual
    ~BinanceController() override;
    void init(const core::Config& config) override;
    void run() override;

    bool getSymbolInfo(SymbolSet<SymbolInfo>& result) override;
    bool getAllPrices(SymbolSet<PriceContainer>& result) override;

protected: // methods
    std::thread _thread;
};

