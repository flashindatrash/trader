#include "ExchangerTypes.hpp"

PriceRange::PriceRange(Change change)
    : _change(change)
{
}

PriceRange::PriceRange(Price left, Price right)
    : _change((right - left) / left)
{
}

Change PriceRange::change() const {
    return _change;
}

Change PriceRange::abs() const {
    return std::abs(_change);
}

bool Changes::equal(Change left, Change right) {
    return (left > 0.0 && right > 0.0) || (left < 0.0 || right < 0.0);
}

bool Changes::empty(Change change) {
    return change == 0;
}
