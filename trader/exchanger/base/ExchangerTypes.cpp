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

static const char* sBuy = "BUY";
static const char* sSell = "SELL";

SideEnum::SideEnum(Side value)
    : _value(value)
{
}

SideEnum::SideEnum(double change)
    : SideEnum(change > 0.0 ? Sell : change < 0.0 ? Buy : Invalid)
{
}

SideEnum::SideEnum(const std::string& value)
    : SideEnum(value == sBuy ? Buy : value == sSell ? Sell : Invalid)
{
}

SideEnum SideEnum::reverse() const {
    if (_value == Sell)
        return Buy;
    else if (_value == Buy)
        return Sell;
    return Invalid;
}

const char* SideEnum::c_str() const {
    if (_value == Buy)
        return sBuy;
    else if (_value == Sell)
        return sSell;
    return "";
}
