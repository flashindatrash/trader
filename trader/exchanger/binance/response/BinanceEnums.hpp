#pragma once

#include <string>

enum ChartInterval : unsigned int;
enum OrderType : unsigned int;
enum OrderSide : unsigned int;

namespace binance {
    OrderSide deserialize_side(const std::string& value);

    std::string serialize(const ChartInterval& value);
    std::string serialize(const OrderType& value);
    std::string serialize(const OrderSide& value);
}

