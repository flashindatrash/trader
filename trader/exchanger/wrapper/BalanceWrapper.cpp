#include "BalanceWrapper.hpp"

BalanceWrapper* BalanceWrapper::create()
{
    BalanceWrapper* wrapper = new BalanceWrapper();
    return wrapper;
}

void BalanceWrapper::set(Quantity free, Quantity locked = 0.0) {
    _free = free;
    _locked = locked;
}

const Quantity& BalanceWrapper::get() const {
    return _free;
}
