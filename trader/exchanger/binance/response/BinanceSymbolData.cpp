#include "BinanceSymbolData.hpp"
#include <json/json.h>
#include <algorithm>

BinanceSymbolData::BinanceSymbolData(const Json::Value& json)
{
    symbol                      = json["symbol"].asString();
    status                      = json["status"].asString();
    baseAsset                   = json["baseAsset"].asString();
    baseAssetPrecision          = atoi(json["baseAssetPrecision"].asString().c_str());
    baseCommissionPrecision     = atoi(json["baseCommissionPrecision"].asString().c_str());
    quoteAsset                  = json["quoteAsset"].asString();
    quoteAssetPrecision         = atoi(json["quoteAssetPrecision"].asString().c_str());
    quoteCommissionPrecision    = atoi(json["quoteCommissionPrecision"].asString().c_str());
    quoteOrderQtyMarketAllowed  = json["quoteOrderQtyMarketAllowed"].asBool();
    quotePrecision              = atoi(json["quotePrecision"].asString().c_str());
    icebergAllowed              = json["icebergAllowed"].asBool();
    isMarginTradingAllowed      = json["isMarginTradingAllowed"].asBool();
    isSpotTradingAllowed        = json["isSpotTradingAllowed"].asBool();
    ocoAllowed                  = json["ocoAllowed"].asBool();

    if (json["orderTypes"].isArray()) {
        for (const auto &orderType : json["orderTypes"])
            _orderTypes.push_back(orderType.asString());
    }

    if (json["filters"].isArray()) {
        for (uint i = 0; i < json["filters"].size(); ++i) {
            const Json::Value& filter = json["filters"][i];
            if (filter["filterType"].asString() == "MIN_NOTIONAL") {
                minNotional.has             = true;
                minNotional.applyToMarket   = filter["applyToMarket"].asBool();
                minNotional.avgPriceMins    = filter["avgPriceMins"].asInt();
                minNotional.minNotional     = atof(filter["minNotional"].asString().c_str());
            }
            else if (filter["filterType"].asString() == "LOT_SIZE") {
                lotSize.has                 = true;
                lotSize.minQty              = atof(filter["minQty"].asString().c_str());
                lotSize.maxQty              = atof(filter["maxQty"].asString().c_str());
                lotSize.stepSize            = atof(filter["stepSize"].asString().c_str());
            }
        }
    }
}

bool BinanceSymbolData::hasOrderType(const std::string& type) const {
    return std::find(_orderTypes.begin(), _orderTypes.end(), type) != _orderTypes.end();
}
