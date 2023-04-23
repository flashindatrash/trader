#pragma once

#include <string>

namespace Json {
    class Value;
}

struct BinanceErrorData {
public: // static
    static const int DISCONNECTED;
    static const int TOO_MANY_REQUESTS;
    static const int INVALID_TIMESTAMP;
    static const int DAILY_REDEEM_AMOUNT_ERROR;

public: // methods
    BinanceErrorData(const Json::Value& json);

    bool has() const;

public: // vars
    int code = 0;
    std::string msg = "";
};

