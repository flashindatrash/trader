#pragma once

namespace core {
    class Config;
}

class ExchangerController {
public: // static
    static ExchangerController* create();

public: // virtual
    virtual ~ExchangerController() = default;
    virtual void init(const core::Config& config) {}
    virtual void run() {}

protected: // methods
    ExchangerController() = default;
};

