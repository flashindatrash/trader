//
// Created by Вадим Проскурин on 14.01.2022.
//

#include "BinanceExchangeData.hpp"
#include <json/json.h>

BinanceExchangeData::BinanceExchangeData(const Json::Value& json) {
    if (json["rateLimits"].isArray()) {
        for (const auto &data: json["rateLimits"])
            rateLimits.push_back(data);
    }

    if (json["symbols"].isArray()) {
        for (const auto &it: json["symbols"]) {
            BinanceSymbolData data(it);
            symbols.insert(std::make_pair(data.symbol, data));
        }
    }
}