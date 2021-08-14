#pragma once

#include <string>

namespace Json {
    class Value;
}

struct BinanceOrderData
{
public: // methods
    BinanceOrderData() = default;
    BinanceOrderData(const Json::Value& json, bool minimized);

    double getPrice() const;

    bool isRejected() const;
    bool isEmpty() const;

public: // vars
    long orderId = 0;
    std::string clientOrderId = "";
    double cummulativeQuoteQty = 0.0;
    double quantity = 0.0;
    std::string side;
    std::string status = "";
    std::string symbol = "";
};


/*
 * {
  "e": "executionReport",        // Event type
  "E": 1499405658658,            // Event time
  "s": "ETHBTC",                 // Symbol
  "c": "mUvoqJxFIILMdfAW5iGSOW", // Client order ID
  "S": "BUY",                    // Side
  "o": "LIMIT",                  // Order type
  "f": "GTC",                    // Time in force
  "q": "1.00000000",             // Order quantity
  "p": "0.10264410",             // Order price
  "P": "0.00000000",             // Stop price
  "F": "0.00000000",             // Iceberg quantity
  "g": -1,                       // OrderListId
  "C": "",                       // Original client order ID; This is the ID of the order being canceled
  "x": "NEW",                    // Current execution type
  "X": "NEW",                    // Current order status
  "r": "NONE",                   // Order reject reason; will be an error code.
  "i": 4293153,                  // Order ID
  "l": "0.00000000",             // Last executed quantity
  "z": "0.00000000",             // Cumulative filled quantity
  "L": "0.00000000",             // Last executed price
  "n": "0",                      // Commission amount
  "N": null,                     // Commission asset
  "T": 1499405658657,            // Transaction time
  "t": -1,                       // Trade ID
  "I": 8641984,                  // Ignore
  "w": true,                     // Is the order on the book?
  "m": false,                    // Is this trade the maker side?
  "M": false,                    // Ignore
  "O": 1499405658657,            // Order creation time
  "Z": "0.00000000",             // Cumulative quote asset transacted quantity
  "Y": "0.00000000",             // Last quote asset transacted quantity (i.e. lastPrice * lastQty)
  "Q": "0.00000000"              // Quote Order Qty
}*/

/*
 * balance 219.334322 USDT
hold to BUY for 38193.54 (current 38301.32)
{
    "C" : "",
    "E" : 1628018748002,
    "F" : "0.00000000",
    "I" : 15036185664,
    "L" : "0.00000000",
    "M" : false,
    "N" : null,
    "O" : 1628018748001,
    "P" : "0.00000000",
    "Q" : "0.00000000",
    "S" : "SELL",
    "T" : 1628018748001,
    "X" : "NEW",
    "Y" : "0.00000000",
    "Z" : "0.00000000",
    "c" : "KW16sBdmC3C74N849CXAyQ",
    "e" : "executionReport",
    "f" : "GTC",
    "g" : -1,
    "i" : 7036491170,
    "l" : "0.00000000",
    "m" : false,
    "n" : "0",
    "o" : "MARKET",
    "p" : "0.00000000",
    "q" : "0.00100000",
    "r" : "NONE",
    "s" : "BTCUSDT",
    "t" : -1,
    "w" : true,
    "x" : "NEW",
    "z" : "0.00000000"
}

SELL 0.001000  for 0.000000
 */

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
