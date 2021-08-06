#pragma once

#include <global.hpp>

struct BinanceErrorData
{
public: // static
    static const int ERROR_CODE_INSUFFICIENT_BALANCE;

public: // methods
    BinanceErrorData(const Json::Value& json);

    bool has() const;

public: // vars
    int code = 0;
    std::string msg = "";
};

