#include "BalanceWrapper.hpp"

BalanceWrapper* BalanceWrapper::create() {
    auto* wrapper = new BalanceWrapper();
    return wrapper;
}

#include "core/Logger.hpp"
void BalanceWrapper::set(Quantity free, Quantity locked) {
    _free = free;
    _locked = locked;
    Logger::info(util::format("%s -> %s", id().c_str(), _free.c_str()));
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
