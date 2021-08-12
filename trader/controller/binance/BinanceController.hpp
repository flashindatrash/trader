#pragma once

#include <thread>
#include "controller/StockController.hpp"

class BinanceController : public StockController {
public: // methods
    BinanceController() = default;

public: // virtual
    ~BinanceController() override;
    void init(const core::Config& config) override;
    void run() override;

protected: // methods
    std::thread _thread;
};

