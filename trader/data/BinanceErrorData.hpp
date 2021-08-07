#pragma once

#include <global.hpp>

struct BinanceErrorData
{
public: // static
    static const int NEW_ORDER_REJECTED;
    static const int INVALID_TIMESTAMP;

public: // methods
    BinanceErrorData(const Json::Value& json);

    bool has() const;

public: // vars
    int code = 0;
    std::string msg = "";
};

