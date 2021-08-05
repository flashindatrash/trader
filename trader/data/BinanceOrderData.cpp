#include "BinanceOrderData.hpp"

#include <json/json.h>
#include "Logger.hpp"

BinanceOrderData::BinanceOrderData(const Json::Value& json, bool minimized) {
    orderId             = atol(json[minimized ? "i" : "orderId"].asString().c_str());
    clientOrderId       = json[minimized ? "c" : "clientOrderId"].asString();
    quantity            = atof(json[minimized ? "q" : "origQty"].asString().c_str());
    cummulativeQuoteQty = atof(json[minimized ? "Z" : "cummulativeQuoteQty"].asString().c_str());
    side                = json[minimized ? "S" : "side"].asString();
    status              = json[minimized ? "X" : "status"].asString();
    symbol              = json[minimized ? "s" : "symbol"].asString();

    if (isInvalid())
        trace("Invalid order:\n%s\n", json.toStyledString().c_str());
}

double BinanceOrderData::getPrice() const {
    return cummulativeQuoteQty / quantity;
}

bool BinanceOrderData::isWeb() const {
    return not clientOrderId.empty() && clientOrderId.substr(0, 4) == "web_";
}

bool BinanceOrderData::isRejected() const {
    return status == "REJECTED";
}

bool BinanceOrderData::isInvalid() const {
    return symbol.empty() || quantity == 0.0;
}

bool BinanceOrderData::isEmpty() const {
    return orderId == 0;
}
