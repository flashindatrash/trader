#include "BinanceOrderData.hpp"

#include <json/json.h>

BinanceOrderData::BinanceOrderData(const Json::Value& json) {
    orderId = atol(json["orderId"].asString().c_str());
    quantity = atof(json["origQty"].asString().c_str());
    cummulativeQuoteQty = atof(json["cummulativeQuoteQty"].asString().c_str());
    side = json["side"].asString();
}
