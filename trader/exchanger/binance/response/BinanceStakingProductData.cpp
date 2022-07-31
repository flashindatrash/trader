//
// Created by Вадим Проскурин on 17.07.2022.
//

#include "BinanceStakingProductData.hpp"
#include <json/json.h>

BinanceStakingProductData::BinanceStakingProductData(const Json::Value& json) {
    projectId = json["projectId"].asString();

    if (json["detail"]) {
        detail.asset_stake     = json["detail"]["asset"].asString();
        detail.asset_reward    = json["detail"]["rewardAsset"].asString();
        detail.apy             = atof(json["detail"]["apy"].asString().c_str());
        detail.duration        = json["detail"]["duration"].asUInt64();
    }

    if (json["quota"]) {
        quota.personal  = atof(json["quota"]["totalPersonalQuota"].asString().c_str());
        quota.minimum   = atof(json["quota"]["minimum"].asString().c_str());
    }
}