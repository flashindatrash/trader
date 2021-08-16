#include "BinanceOrderData.hpp"
#include <json/json.h>

BinanceOrderData::BinanceOrderData(const Json::Value& json, bool minimized) {
    orderId             = atol(json[minimized ? "i" : "orderId"].asString().c_str());
    id                  = json[minimized ? "c" : "clientOrderId"].asString();
    quantity            = atof(json[minimized ? "q" : "origQty"].asString().c_str());
    quoute_quantity     = atof(json[minimized ? "Z" : "cummulativeQuoteQty"].asString().c_str());
    side                = json[minimized ? "S" : "side"].asString();
    status              = json[minimized ? "X" : "status"].asString();
    symbol              = json[minimized ? "s" : "symbol"].asString();
}

bool BinanceOrderData::isRejected() const {
    return status == "REJECTED";
}
