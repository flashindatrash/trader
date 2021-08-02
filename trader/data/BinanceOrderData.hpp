#pragma once

#include <string>

namespace Json {
    class Value;
}

struct BinanceOrderData
{
public: // methods
    BinanceOrderData(const Json::Value& json);

public: // vars
    long orderId = 0;
    double cummulativeQuoteQty = 0.0;
    double quantity = 0.0;
    std::string side = "";
};


/*
 * BUY
 *
 * {
    "clientOrderId" : "i78I0iX6qeUPPOIOlwxVeU",
    "cummulativeQuoteQty" : "11.13885270",
    "executedQty" : "0.00027000",
    "fills" :
    [
        {
            "commission" : "0.00002526",
            "commissionAsset" : "BNB",
            "price" : "41255.01000000",
            "qty" : "0.00027000",
            "tradeId" : 980526217
        }
    ],
    "orderId" : 7003952483,
    "orderListId" : -1,
    "origQty" : "0.00027000",
    "price" : "0.00000000",
    "side" : "BUY",
    "status" : "FILLED",
    "symbol" : "BTCUSDT",
    "timeInForce" : "GTC",
    "transactTime" : 1627761760610,
    "type" : "MARKET"
}
*/

/*
 * get
 *
 * [
    {
        "clientOrderId" : "WhmGkHz6jYihPVBFabXI1y",
        "cummulativeQuoteQty" : "12.50711700",
        "executedQty" : "0.00030000",
        "icebergQty" : "0.00000000",
        "isWorking" : true,
        "orderId" : 7005908573,
        "orderListId" : -1,
        "origQty" : "0.00030000",
        "origQuoteOrderQty" : "0.00000000",
        "price" : "0.00000000",
        "side" : "SELL",
        "status" : "FILLED",
        "stopPrice" : "0.00000000",
        "symbol" : "BTCUSDT",
        "time" : 1627778695381,
        "timeInForce" : "GTC",
        "type" : "MARKET",
        "updateTime" : 1627778695381
    }
]
 *
 */
