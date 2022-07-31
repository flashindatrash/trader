//
// Created by Вадим Проскурин on 17.07.2022.
//

#pragma once

#include "exchanger/wrapper/StakingWrapper.hpp"

namespace Json {
    class Value;
}

struct BinanceStakingProductData {
public: // methods
    BinanceStakingProductData() = default;
    BinanceStakingProductData(const Json::Value& json);

public: // vars
    std::string projectId;
    StakingDetail detail;
    StakingQuota quota;
};


