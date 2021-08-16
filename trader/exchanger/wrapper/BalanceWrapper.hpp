#pragma once

#include "exchanger/base/Identifier.hpp"

class BalanceWrapper : public Identifier {
public: // static
    static BalanceWrapper* create();

public: // methods
    void set(double free, double locked);
    const double& get() const;

protected: // methods
    BalanceWrapper() = default;

protected: // vars
    double _free = 0.0;
    double _locked = 0.0;
};

