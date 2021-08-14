#include "SideEnum.hpp"

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
