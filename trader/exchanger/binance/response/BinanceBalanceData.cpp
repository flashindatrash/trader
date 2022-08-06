#include "BinanceBalanceData.hpp"
#include <json/json.h>

BinanceBalanceData::BinanceBalanceData(const Json::Value& json, const std::string& asset_key, const std::string& amount_key, const std::string& locked_key)
{
    asset = json[asset_key].asString();
    free = Decimal::deserialize(json[amount_key].asString());
    if (not locked_key.empty())
        locked = Decimal::deserialize(json[locked_key].asString());
}
