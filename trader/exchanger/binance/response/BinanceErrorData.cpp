#include "BinanceErrorData.hpp"
#include "core/Logger.hpp"
#include <json/json.h>

const int BinanceErrorData::DISCONNECTED = -1001;
const int BinanceErrorData::TOO_MANY_REQUESTS = -1003;
const int BinanceErrorData::INVALID_TIMESTAMP = -1021;
const int BinanceErrorData::NEW_ORDER_REJECTED = -2010;

BinanceErrorData::BinanceErrorData(const Json::Value& json, std::string context) {
    if (not context.empty())
        context = context + ": ";

    if (json.isNull()) {
        code = -1;
        msg = context + "empty json";
        return;
    }

    if (not json.isObject() || not json["code"] || not json["code"].isInt())
        return;

    code = json["code"].asInt();
    msg = context + json["msg"].asString();

    // fixme
    if (code == TOO_MANY_REQUESTS)
        Logger::error(msg);
}

bool BinanceErrorData::has() const {

    return code != 0;
}
