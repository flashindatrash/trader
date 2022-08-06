#include "BalanceWrapper.hpp"

BalanceWrapper* BalanceWrapper::create() {
    auto* wrapper = new BalanceWrapper();
    return wrapper;
}

void BalanceWrapper::set(Quantity free, Quantity locked) {
    _free = free;
    _locked = locked;
}

const Quantity& BalanceWrapper::get() const {
    return _free;
}

void BalanceWrapper::spend(Quantity count) {
    _free -= count;
}

void BalanceWrapper::gain(Quantity count) {
    _free += count;
}
