#pragma once

#include "Proxy.hpp"

struct BinanceBalanceData;

class BinanceAccount : public Core::Proxy<BinanceAccount>, public Core::Emitter<BinanceBalanceData>
{
public: // methods
    BinanceAccount() {}

    void init();
    void connect();

    double getBalance(const std::string& asset) const;

protected: // methods
    int handle(Json::Value& json);

    void setBalance(const BinanceBalanceData& data);

protected: // vars
    std::string _listenKey = "";
    std::unordered_map<std::string, double> _balance;
};

#define SAccount() BinanceAccount::getInstance()

/* onUpdateData на продаже
 * {
    "C" : "",
    "E" : 1627847019425,
    "F" : "0.00000000",
    "I" : 14987970118,
    "L" : "0.00000000",
    "M" : false,
    "N" : null,
    "O" : 1627847019424,
    "P" : "0.00000000",
    "Q" : "0.00000000",
    "S" : "SELL",
    "T" : 1627847019424,
    "X" : "NEW",
    "Y" : "0.00000000",
    "Z" : "0.00000000",
    "c" : "web_f6347814ef2544f996827fa474b5c555",
    "e" : "executionReport",
    "f" : "GTC",
    "g" : -1,
    "i" : 7013692896,
    "l" : "0.00000000",
    "m" : false,
    "n" : "0",
    "o" : "MARKET",
    "p" : "0.00000000",
    "q" : "0.00026600",
    "r" : "NONE",
    "s" : "BTCUSDT",
    "t" : -1,
    "w" : true,
    "x" : "NEW",
    "z" : "0.00000000"
}

{
    "C" : "",
    "E" : 1627847019425,
    "F" : "0.00000000",
    "I" : 14987970119,
    "L" : "41443.32000000",
    "M" : true,
    "N" : "BNB",
    "O" : 1627847019424,
    "P" : "0.00000000",
    "Q" : "0.00000000",
    "S" : "SELL",
    "T" : 1627847019424,
    "X" : "FILLED",
    "Y" : "11.02392312",
    "Z" : "11.02392312",
    "c" : "web_f6347814ef2544f996827fa474b5c555",
    "e" : "executionReport",
    "f" : "GTC",
    "g" : -1,
    "i" : 7013692896,
    "l" : "0.00026600",
    "m" : false,
    "n" : "0.00002440",
    "o" : "MARKET",
    "p" : "0.00000000",
    "q" : "0.00026600",
    "r" : "NONE",
    "s" : "BTCUSDT",
    "t" : 982028270,
    "w" : false,
    "x" : "TRADE",
    "z" : "0.00026600"
}

{
    "B" :
    [
        {
            "a" : "BTC",
            "f" : "0.00035313",
            "l" : "0.00000000"
        },
        {
            "a" : "BNB",
            "f" : "0.38972220",
            "l" : "0.00000000"
        },
        {
            "a" : "USDT",
            "f" : "65.52717463",
            "l" : "0.00000000"
        }
    ],
    "E" : 1627847019425,
    "e" : "outboundAccountPosition",
    "u" : 1627847019424
}
 *
 */
