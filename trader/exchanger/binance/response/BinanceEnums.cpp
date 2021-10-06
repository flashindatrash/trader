#include "BinanceEnums.hpp"

#include "exchanger/wrapper/ChartWrapper.hpp"
#include "exchanger/wrapper/OrderWrapper.hpp"

OrderSide binance::deserialize_side(const std::string& value) {
    if (value == "BUY") return OrderSide::Buy;
    if (value == "SELL") return OrderSide::Sell;
    return OrderSide::Invalid;
}

std::string binance::serialize(const ChartInterval& interval) {
    switch (interval) {
        case ChartInterval::m5: return "5m";
        case ChartInterval::m15: return "15m";
        case ChartInterval::d1: return "1d";
        default: return "";
    }
}

std::string binance::serialize(const OrderType& type) {
    switch (type) {
        case OrderType::Market: return "MARKET";
        default: return "";
    }
}

std::string binance::serialize(const OrderSide& type) {
    switch (type) {
        case OrderSide::Sell: return "SELL";
        case OrderSide::Buy: return "BUY";
        default: return "";
    }
}
