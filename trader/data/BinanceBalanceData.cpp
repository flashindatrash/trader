#include "BinanceBalanceData.hpp"

#include <json/json.h>

BinanceBalanceData::BinanceBalanceData(const Json::Value& json, bool minimized)
{
    asset = json[minimized ? "a" : "asset"].asString();
    free = atof(json[minimized ? "f" : "free"].asString().c_str());
    locked = atof(json[minimized ? "l" : "locked"].asString().c_str());
}

BinanceBalanceData::BinanceBalanceData(std::string a, double f)
    : asset(a)
    , free(f)
{
}
