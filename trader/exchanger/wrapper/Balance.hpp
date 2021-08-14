#pragma once


class Balance {
public: // static
    static Balance* create();

public: // methods
    void set(double free, double locked);
    const double& get() const;

protected: // methods
    Balance() = default;

protected: // vars
    double _free = 0.0;
    double _locked = 0.0;
};

