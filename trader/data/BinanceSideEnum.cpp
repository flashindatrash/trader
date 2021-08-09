#include "BinanceSideEnum.hpp"

static const char* sBuy = "BUY";
static const char* sSell = "SELL";

BinanceSideEnum::BinanceSideEnum(Side value)
    : _value(value)
{
}

BinanceSideEnum::BinanceSideEnum(double change)
    : BinanceSideEnum(change > 0.0 ? Sell : change < 0.0 ? Buy : Invalid)
{
}

BinanceSideEnum::BinanceSideEnum(const std::string& value)
    : BinanceSideEnum(value == sBuy ? Buy : value == sSell ? Sell : Invalid)
{
}

BinanceSideEnum BinanceSideEnum::reverse() const {
    if (_value == Sell)
        return Buy;
    else if (_value == Buy)
        return Sell;
    return Invalid;
}

const char* BinanceSideEnum::c_str() const {
    if (_value == Buy)
        return sBuy;
    else if (_value == Sell)
        return sSell;
    return "";
}
