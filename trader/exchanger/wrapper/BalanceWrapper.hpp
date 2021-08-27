#pragma once

#include "exchanger/base/Identifier.hpp"
#include "exchanger/base/OrderBase.hpp"

class BalanceWrapper : public Identifier {
public: // static
    static BalanceWrapper* create();

public: // methods
    void set(Quantity free, Quantity locked);
    const Quantity& get() const;

protected: // methods
    BalanceWrapper() = default;

protected: // vars
    Quantity _free = 0.0;
    Quantity _locked = 0.0;
};

