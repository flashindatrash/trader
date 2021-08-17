#pragma once

#include <string>

namespace Json {
    class Value;
}

struct BinanceErrorData {
public: // static
    static const int DISCONNECTED;
    static const int INVALID_TIMESTAMP;
    static const int NEW_ORDER_REJECTED;

public: // methods
    BinanceErrorData(const Json::Value& json, std::string context = "");

    bool has() const;

public: // vars
    int code = 0;
    std::string msg = "";
};

