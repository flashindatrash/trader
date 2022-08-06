#include "BinanceOrderData.hpp"
#include "BinanceEnums.hpp"
#include <json/json.h>

BinanceOrderData::BinanceOrderData(const Json::Value& json, bool minimized) {
    orderId             = atol(json[minimized ? "i" : "orderId"].asString().c_str());
    id                  = json[minimized ? "c" : "clientOrderId"].asString();
    base_quantity       = Decimal::deserialize(json[minimized ? "q" : "origQty"].asString());
    quote_quantity      = Decimal::deserialize(json[minimized ? "Z" : "cummulativeQuoteQty"].asString());
    side                = binance::deserialize_side(json[minimized ? "S" : "side"].asString());
    status              = json[minimized ? "X" : "status"].asString();
    symbol              = json[minimized ? "s" : "symbol"].asString();

    if (not minimized && json["fills"].isArray()) {
        for (const auto &fill: json["fills"])
            fills.emplace_back(fill);
    }
}

bool BinanceOrderData::isRejected() const {
    return status == "REJECTED";
}
