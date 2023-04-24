//
// Created by Вадим Проскурин on 17.07.2022.
//

#include "BinanceStakingProductData.hpp"
#include <json/json.h>

BinanceStakingProductData::BinanceStakingProductData(const Json::Value& json) {
    projectId = json["projectId"].asString();

    if (json["detail"].isObject()) {
        detail.asset_stake     = json["detail"]["asset"].asString();
        detail.asset_reward    = json["detail"]["rewardAsset"].asString();
        detail.apy             = atof(json["detail"]["apy"].asString().c_str());
        detail.duration        = json["detail"]["duration"].asUInt64();
    }

    if (json["quota"].isObject()) {
        quota.personal  = Decimal::deserialize(json["quota"]["totalPersonalQuota"].asString());
        quota.minimum   = Decimal::deserialize(json["quota"]["minimum"].asString());
    }
}