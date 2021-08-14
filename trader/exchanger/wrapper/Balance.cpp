#include "Balance.hpp"

Balance* Balance::create()
{
    Balance* wrapper = new Balance();
    return wrapper;
}

void Balance::set(double free, double locked = 0.0) {
    _free = free;
    _locked = locked;
}

const double& Balance::get() const {
    return _free;
}
