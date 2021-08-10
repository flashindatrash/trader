#include <global.hpp>
#include "wrapper/PriceBase.hpp"

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
