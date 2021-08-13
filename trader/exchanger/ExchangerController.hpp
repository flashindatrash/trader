#pragma once

#include "wrapper/Symbol.hpp"

namespace core {
    class Config;
}

class ExchangerController {
public: // static
    static ExchangerController* create();

public: // virtual
    virtual ~ExchangerController() = default;
    virtual void init(const core::Config& config) = 0;
    virtual void run() = 0;

    virtual std::vector<std::pair<Symbol, Price>> getAllPrices() = 0;

protected: // methods
    ExchangerController() = default;
};

