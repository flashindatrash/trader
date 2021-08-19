#include "BinanceWebsocket.hpp"
#include "binacpp_websocket.h"
#include <json/json.h>
#include <functional>
#include "Logger.hpp"
#include "response/BinanceErrorData.hpp"

BinanceWebsocket::BinanceWebsocket(std::string path)
    : path(path)
{
}

void BinanceWebsocket::connect() {
    connected = true;
    std::string endpoint = "/ws/" + path;
    BinaCPP_websocket::connect_endpoint(std::bind(&BinanceWebsocket::handler, this, std::placeholders::_1), endpoint.c_str());
}

int BinanceWebsocket::handler(Json::Value& json) {
    BinanceErrorData error(json, "BinanceWebsocket::handler(" + path + ")");
    if (error.has()) {
        Logger::error(error.msg.c_str());
        if (error.code == BinanceErrorData::DISCONNECTED)
            connected = false;
        return 0;
    }

    onData.emmit(json);
    return 0;
}
