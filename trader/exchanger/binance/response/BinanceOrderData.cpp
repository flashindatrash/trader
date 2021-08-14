#include "BinanceOrderData.hpp"
#include <json/json.h>

BinanceOrderData::BinanceOrderData(const Json::Value& json, bool minimized) {
    orderId             = atol(json[minimized ? "i" : "orderId"].asString().c_str());
    clientOrderId       = json[minimized ? "c" : "clientOrderId"].asString();
    quantity            = atof(json[minimized ? "q" : "origQty"].asString().c_str());
    cummulativeQuoteQty = atof(json[minimized ? "Z" : "cummulativeQuoteQty"].asString().c_str());
    side                = json[minimized ? "S" : "side"].asString();
    status              = json[minimized ? "X" : "status"].asString();
    symbol              = json[minimized ? "s" : "symbol"].asString();
}

double BinanceOrderData::getPrice() const {
    return cummulativeQuoteQty / quantity;
}

bool BinanceOrderData::isRejected() const {
    return status == "REJECTED";
}

bool BinanceOrderData::isEmpty() const {
    return status.empty();
}
