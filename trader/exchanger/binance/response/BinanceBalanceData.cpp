#include "BinanceBalanceData.hpp"
#include <json/json.h>

BinanceBalanceData::BinanceBalanceData(const Json::Value& json, const std::string& asset_key, const std::string& amount_key, const std::string& locked_key)
{
    asset = json[asset_key].asString();
    free = atof(json[amount_key].asString().c_str());
    if (not locked_key.empty())
        locked = atof(json[locked_key].asString().c_str());
}

BinanceBalanceData::BinanceBalanceData(std::string a, double f)
    : asset(a)
    , free(f)
{
}
