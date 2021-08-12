#pragma once

namespace core {
    class Config;
}

class StockController {
public: // static
    static StockController* create();

public: // virtual
    virtual ~StockController() = default;
    virtual void init(const core::Config& config) {}
    virtual void run() {}

protected: // methods
    StockController() = default;
};

