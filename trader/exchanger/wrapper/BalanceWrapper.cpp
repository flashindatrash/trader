#include "BalanceWrapper.hpp"

BalanceWrapper* BalanceWrapper::create()
{
    BalanceWrapper* wrapper = new BalanceWrapper();
    return wrapper;
}

void BalanceWrapper::set(double free, double locked = 0.0) {
    _free = free;
    _locked = locked;
}

const double& BalanceWrapper::get() const {
    return _free;
}
